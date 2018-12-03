#include "include/sql_parser/CsvSQLParser.h"
#include "include/sql_parser/SQLiteParserAdapter.h"
#include "include/common/DBInterface.h"
#include <QFileInfo>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlRecord>
#include <QSqlDriver>
#include <QDebug>
#include <map>
#define VERSION 0

struct SourceParser
{
    SourceParser(std::string& path) : _currIndex(0),
        _currRowID(0),
        _lastPack(false),
        _path(path)
    {
        createOrReloadSource();
    }

    bool createOrReloadSource()
    {
        std::shared_ptr<QFileInfo> pFile = std::shared_ptr<QFileInfo>(new QFileInfo(QString(_path.c_str())));
        if (!pFile->exists())
            throw std::runtime_error("the file is not exist");
         _tableName = pFile->baseName().toStdString();
        _pExtractor = std::shared_ptr<CsvExtractor>(new CsvExtractor(_path));
        _pExtractor->selectTab(_pExtractor->getTabList()->at(0));
        _currIndex = 0;
        _lastPack = false;
        _curr_pack_data.clear();
        return _pExtractor->parseData(_curr_pack_data, _lastPack);
    }
    bool eof()
    {
        return _lastPack && (_curr_pack_data.size() == 0 ||
                             _curr_pack_data.at(0)->size() == _currIndex);
    }

    bool getCurValue(int col, std::string& curValue)
    {
        curValue = _curr_pack_data.at(col)->at(_currIndex);
        if (col >= _curr_pack_data.size())
            return false;
        return true;
    }

    bool moveNext()
    {
        _currIndex++;
        if (eof())
            return false;
        if (_curr_pack_data.at(0)->size() == _currIndex)
        {
            _currIndex = 0;
            return _pExtractor->parseData(_curr_pack_data, _lastPack);
        }
        return true;
    }

    int rowID()
    {
        return _currRowID++;
    }

    std::vector<TableColumnInfo>& metaData()
    {
        return _pExtractor->getMetaData();
    }

    int _currIndex;
    int _currRowID;
    std::string _tableName;
    std::string _path;
    bool _lastPack;
    outdata_t _curr_pack_data;
    std::shared_ptr<Extractor> _pExtractor;
};

struct  sqlite3_vtab_csv : sqlite3_vtab
{
    SourceParser*  p_csvWrapper;
};

struct  sqlite3_cursor_csv : sqlite3_vtab_cursor
{
    SourceParser* p_csvWrapper;
};


std::shared_ptr<CsvSQLParser> CsvSQLParser::_instance;
char CsvSQLParser::csv[] = "csv";
CsvSQLParser::CsvSQLParser() : initilized(false)
{
}

std::shared_ptr<CsvSQLParser> CsvSQLParser::getInstance()
{
    if (_instance)
        return _instance;
    _instance = std::shared_ptr<CsvSQLParser>(new CsvSQLParser);
    return _instance;
}


sqlite3* CsvSQLParser::parse(std::string& filePath)
{
    std::shared_ptr<SQLiteParserAdapter<csv, VERSION>> instance = SQLiteParserAdapter<csv, VERSION>::getInstance();
    std::shared_ptr<QFileInfo> pFile = std::shared_ptr<QFileInfo>(new QFileInfo(QString(filePath.c_str())));
    std::string tableName = pFile->baseName().toStdString();
    if (!pFile->exists())
        return nullptr;

    if (_fileMap.find(tableName) != _fileMap.end())
        return nullptr;
    _fileMap[tableName] = filePath;

    sqlite3 *db = SQLiteParserDB::getInstance()->getDB();
    instance->createVirtualTable(tableName);
    return db;
}

void CsvSQLParser::initilize()
{
    if (initilized)
        return;
    std::shared_ptr<SQLiteParserAdapter<csv, VERSION>> instance = SQLiteParserAdapter<csv, VERSION>::getInstance();
    instance->setCreateCallBack([this](sqlite3*, void *pAux,
                                 int argc, const char *const*argv,
                                 sqlite3_vtab **ppVTab, char**,
                                std::vector<SQLiteParserAdapter<csv, VERSION>::ColumnInfo>& colInfos) -> int
    {

        if (argc < 3)
            return SQLITE_INTERNAL;
        const char *name = argv[2];
        std::string strName(name);
        std::string filePath = _fileMap[name];
        sqlite3_vtab_csv* vtab = (sqlite3_vtab_csv*)sqlite3_malloc(sizeof(sqlite3_vtab_csv));
        vtab->p_csvWrapper = new SourceParser(filePath);
        *ppVTab = vtab;
        std::vector<TableColumnInfo>& vecInfo = vtab->p_csvWrapper->metaData();
        for (int i = 0; i < vecInfo.size(); i++)
        {
            SQLiteParserAdapter<csv, VERSION>::ColumnInfo info;
            info.colName = vecInfo.at(i).colName;
            info.type = SQLiteParserAdapter<csv, VERSION>::TEXT;
            colInfos.push_back(info);
        }
        return SQLITE_OK;
    });

    instance->setConnectCallBack([](sqlite3*, void *pAux,
                                 int argc, const char *const*argv,
                                 sqlite3_vtab **ppVTab, char**) -> int
    {
        return SQLITE_OK;
    });

    instance->setOpenCallBack([](sqlite3_vtab *pVTab, sqlite3_vtab_cursor **ppCursor) -> int
    {
        sqlite3_vtab_csv* vtab = static_cast<sqlite3_vtab_csv*>(pVTab);
        sqlite3_cursor_csv *pCur;
        pCur = (sqlite3_cursor_csv*)sqlite3_malloc64( sizeof(sqlite3_cursor_csv));
        pCur->p_csvWrapper = vtab->p_csvWrapper;
        *ppCursor = pCur;
        pCur->p_csvWrapper->createOrReloadSource();
        return SQLITE_OK;
    });

    instance->setCloseCallBack([](sqlite3_vtab_cursor* cur) -> int
    {
        sqlite3_free(cur);
        return SQLITE_OK;
    });
    instance->setDisconnectCallBack([](sqlite3_vtab *pVTab) -> int
    {
        sqlite3_vtab_csv* vtab = static_cast<sqlite3_vtab_csv*>(pVTab);
        delete vtab->p_csvWrapper;
        vtab->p_csvWrapper = nullptr;
        sqlite3_free(pVTab);
        return SQLITE_OK;
    });
    instance->setDestroyCallBack([](sqlite3_vtab *pVTab) -> int
    {
        sqlite3_vtab_csv* vtab = static_cast<sqlite3_vtab_csv*>(pVTab);
        delete vtab->p_csvWrapper;
        vtab->p_csvWrapper = nullptr;
        sqlite3_free(pVTab);
        return SQLITE_OK;
    });
    instance->setRowidCallBack([](sqlite3_vtab_cursor* pCur, sqlite3_int64 *pRowid) -> int
    {
        sqlite3_cursor_csv* cur = static_cast<sqlite3_cursor_csv*>(pCur);
        *pRowid = cur->p_csvWrapper->rowID();
        return SQLITE_OK;
    });
    instance->setEofCallBack([](sqlite3_vtab_cursor* pCur) -> int
    {
        sqlite3_cursor_csv* cur = static_cast<sqlite3_cursor_csv*>(pCur);
        return cur->p_csvWrapper->eof();
    });

    instance->setColumnCallBack([](sqlite3_vtab_cursor* pCur, sqlite3_context *ctx, int idx) -> int
    {
        sqlite3_cursor_csv* cur = static_cast<sqlite3_cursor_csv*>(pCur);
        std::string curValue;
        if (cur->p_csvWrapper->getCurValue(idx, curValue))
        {
          sqlite3_result_text(ctx, curValue.c_str(), -1, SQLITE_STATIC);
        }
        return SQLITE_OK;
    });
    instance->setNextCallBack([](sqlite3_vtab_cursor* pCur) -> int
    {
        sqlite3_cursor_csv* cur = static_cast<sqlite3_cursor_csv*>(pCur);
        cur->p_csvWrapper->moveNext();
        return SQLITE_OK;
    });
    instance->setBestIndexCallBack([this](sqlite3_vtab *pVTab, sqlite3_index_info* index) -> int
    {
//        int i;
//        int nConst = 0;
//         for(i=0; i<index->nConstraint; i++){
//             unsigned char op;
//             if( index->aConstraint[i].usable==0 ) continue;
//             index->estimatedCost = 10;
//             index->aConstraintUsage[nConst].argvIndex = nConst+1;
//             nConst++;
//           }

//        if (index->aConstraint->usable == 0)
//        {
//            index->idxNum = -1;
//            return SQLITE_OK;
//        }
//        sqlite3_vtab_csv_extractor* vtab = static_cast<sqlite3_vtab_csv_extractor*>(pVTab);
//        vtab->p_csvWrapper->_path;
//        index->idxNum = 1;
//        index->aConstraintUsage[0].argvIndex = 1;
        return SQLITE_OK;
    });
    instance->setFilterCallBack([this](sqlite3_vtab_cursor* pCur, int idxNum, const char *idxStr,
                                int argc, sqlite3_value **argv) -> int
    {
        sqlite3_cursor_csv* cur = static_cast<sqlite3_cursor_csv*>(pCur);
        cur->p_csvWrapper->createOrReloadSource();
        return SQLITE_OK;
    });
    instance->startUp();
    initilized = true;
}


