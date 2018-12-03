#ifndef SQLiteParserAdapter_H
#define SQLiteParserAdapter_H

#pragma once
// This is for SQLite virtual table. you can query every thing by implement the adapter
#include <memory>
#include <functional>
#include <vector>
#include "sqlite3.h"
template <char const* moduleName>
class SQLiteParserAdapter
{
public:
    enum ColumnType
    {
        TEXT,
        NUMERIC,
        INTEGER,
        REAL,
        BLOB,
    };

    struct ColumnInfo
    {
        ColumnType type;
        std::string colName;
    };

    bool startUp(sqlite3* dbPtr)
    {
        if (_startUped)
            return true;
        if (!dbPtr)
            return false;
        _dbPtr = dbPtr;
        _startUped = true;
        int rc = sqlite3_create_module(dbPtr, moduleName, &_sqlite3_module, 0);
        return rc == SQLITE_OK;
    }

    bool createSchema(std::vector<ColumnInfo>& colInfos)
    {
        if (!_startUped)
            return false;
        std::string schemaStr = "CREATE TABLE X(";
        for (auto idx = 0; idx < colInfos.size(); idx++)
        {
            schemaStr += colInfos.at(idx).colName;
            schemaStr += " ";
            schemaStr += strFromType(colInfos.at(idx).type);
            if (idx != colInfos.size() - 1)
                schemaStr += ", ";
        }
        schemaStr += ")";
        int rc = sqlite3_declare_vtab(_dbPtr, schemaStr.c_str());
        return rc == SQLITE_OK;
    }

	static std::shared_ptr<SQLiteParserAdapter<moduleName>> getInstance()
	{
		if (_instance)
			return _instance;
        _instance = std::shared_ptr<SQLiteParserAdapter<moduleName>>(new SQLiteParserAdapter<moduleName>());
		return _instance;
	}

	typedef std::function<int(sqlite3*, void *pAux,
		int argc, const char *const*argv,
		sqlite3_vtab **ppVTab, char**)> CreateCallBack;
	typedef std::function<int(sqlite3*, void *pAux,
		int argc, const char *const*argv,
		sqlite3_vtab **ppVTab, char**)> ConnectCallBack;
	typedef std::function<int(sqlite3_vtab *pVTab, sqlite3_index_info*)> BestIndexCallBack;
	typedef std::function<int(sqlite3_vtab *pVTab)> DisconnectCallBack;
	typedef std::function<int(sqlite3_vtab *pVTab)> DestroyCallBack;
	typedef std::function<int(sqlite3_vtab *pVTab, sqlite3_vtab_cursor **ppCursor)> OpenCallBack;
	typedef std::function<int(sqlite3_vtab_cursor*)> CloseCallBack;
	typedef std::function<int(sqlite3_vtab_cursor*, int idxNum, const char *idxStr,
		int argc, sqlite3_value **argv)> FilterCallBack;
	typedef std::function<int(sqlite3_vtab_cursor*)> NextCallBack;
	typedef std::function<int(sqlite3_vtab_cursor*)> EofCallBack;
	typedef std::function<int(sqlite3_vtab_cursor*, sqlite3_context*, int)> ColumnCallBack;
	typedef std::function<int(sqlite3_vtab_cursor*, sqlite3_int64 *pRowid)> RowidCallBack;
	typedef std::function<int(sqlite3_vtab *, int, sqlite3_value **, sqlite3_int64 *)> UpdateCallBack;
	typedef std::function<int(sqlite3_vtab *pVTab)> BeginCallBack;
	typedef std::function<int(sqlite3_vtab *pVTab)> SyncCallBack;
	typedef std::function<int(sqlite3_vtab *pVTab)> CommitCallBack;
	typedef std::function<int(sqlite3_vtab *pVTab)> RollbackCallBack;
	typedef std::function<int(sqlite3_vtab *pVtab, int nArg, const char *zName,
		void(**pxFunc)(sqlite3_context*, int, sqlite3_value**),
		void **ppArg)> FindFunctionCallBack;
	typedef std::function<int(sqlite3_vtab *pVtab, const char *zNew)> RenameCallBack;
	typedef std::function<int(sqlite3_vtab *pVTab, int)> SavepointCallBack;
	typedef std::function<int(sqlite3_vtab *pVTab, int)> ReleaseCallBack;
	typedef std::function<int(sqlite3_vtab *pVTab, int)> RollbackToCallBack;

    void setCreateCallBack(CreateCallBack ceateCallBack)
	{
		_createCallBack = ceateCallBack;
        _sqlite3_module.xCreate = xCreate;

	}
    void setConnectCallBack(ConnectCallBack connectCallBack)
	{
		_connectCallBack = connectCallBack;
        _sqlite3_module.xConnect = xConnect;
	}
    void setBestIndexCallBack(BestIndexCallBack bestIndexCallBack)
	{
		_bestIndexCallBack = bestIndexCallBack;
        _sqlite3_module.xBestIndex = xBestIndex;
	}
    void setDisconnectCallBack(DisconnectCallBack disconnectCallBack)
	{
		_disconnectCallBack = disconnectCallBack;
        _sqlite3_module.xDisconnect = xDisconnect;
	}
    void setDestroyCallBack(DestroyCallBack destroyCallBack)
	{
		_destroyCallBack = destroyCallBack;
        _sqlite3_module.xDestroy = xDestroy;
	}
    void setOpenCallBack(OpenCallBack openCallBack)
	{
		_openCallBack = openCallBack;
        _sqlite3_module.xOpen = xOpen;
	}
    void setCloseCallBack(CloseCallBack closeCallBack)
	{
		_closeCallBack = closeCallBack;
        _sqlite3_module.xClose = xClose;
	}
    void setFilterCallBack(FilterCallBack filterCallBack)
	{
		_filterCallBack = filterCallBack;
        _sqlite3_module.xFilter = xFilter;
	}
    void setNextCallBack(NextCallBack nextCallBack)
    {
        _nextCallBack = nextCallBack;
        _sqlite3_module.xNext = xNext;
    }
    void setEofCallBack(EofCallBack eofCallBack)
	{
		_eofCallBack = eofCallBack;
        _sqlite3_module.xEof = xEof;
	}
    void setColumnCallBack(ColumnCallBack columnCallBack)
	{
		_columnCallBack = columnCallBack;
        _sqlite3_module.xColumn = xColumn;
	}
    void setRowidCallBack(RowidCallBack rowidCallBack)
	{
		_rowidCallBack = rowidCallBack;
        _sqlite3_module.xRowid = xRowid;
	}
    void setUpdateCallBack(UpdateCallBack updateCallBack)
	{
		_updateCallBack = updateCallBack;
        _sqlite3_module.xUpdate = xUpdate;
	}
    void setBeginCallBack(BeginCallBack beginCallBack)
	{
		_beginCallBack = beginCallBack;
        _sqlite3_module.xBegin = xBegin;
	}
    void setSyncCallBack(SyncCallBack syncCallBack)
	{
		_syncCallBack = syncCallBack;
        _sqlite3_module.xSync = xSync;
	}
    void setCommitCallBack(CommitCallBack commitCallBack)
	{
		_commitCallBack = commitCallBack;
        _sqlite3_module.xCommit = xCommit;
	}
    void setRollbackCallBack(RollbackCallBack rollbackCallBack)
	{
		_rollbackCallBack = rollbackCallBack;
        _sqlite3_module.xRollback = xRollback;
	}
    void setFindFunctionCallBack(FindFunctionCallBack findFunctionCallBack)
	{
		_findFunctionCallBack = findFunctionCallBack;
        _sqlite3_module.xFindFunction = xFindFunction;
	}
    void setRenameCallBack(RenameCallBack renameCallBack)
	{
		_renameCallBack = renameCallBack;
        _sqlite3_module.xRename = xRename;
	}
    void setSavepointCallBack(SavepointCallBack savepointCallBack)
	{
		_savepointCallBack = savepointCallBack;
        _sqlite3_module.xSavepoint = xSavepoint;
	}
    void setReleaseCallBack(ReleaseCallBack releaseCallBack)
	{
		_releaseCallBack = releaseCallBack;
        _sqlite3_module.xRelease = xRelease;
	}
    void setRollbackToCallBack(RollbackToCallBack rollbackToCallBack)
	{
		_rollbackToCallBack = rollbackToCallBack;
        _sqlite3_module.xRollback = xRollback;
	}

private:
    static std::string strFromType(ColumnType type)
    {
        switch (type)
        {
        case TEXT:
            return "TEXT";
        case NUMERIC:
            return "NUMERIC";
        case INTEGER:
            return "INTEGER";
        case REAL:
            return "REAL";
        case BLOB:
            return "BLOB";
        }
        return "UNKNOWN";
    }

    SQLiteParserAdapter::SQLiteParserAdapter() : _startUped(false)
	{
        _sqlite3_module = {};
//        _pSqlite3_module = std::shared_ptr<sqlite3_module>(new sqlite3_module);
//        _pSqlite3_module->iVersion = 0;
//        _pSqlite3_module->xCreate = nullptr;
//        _pSqlite3_module->xConnect = nullptr;
//        _pSqlite3_module->xBestIndex = nullptr;
//        _pSqlite3_module->xDisconnect = nullptr;
//        _pSqlite3_module->xDestroy = nullptr;
//        _pSqlite3_module->xOpen = nullptr;
//        _pSqlite3_module->xClose = nullptr;
//        _pSqlite3_module->xFilter = nullptr;
//        _pSqlite3_module->xNext = nullptr;
//        _pSqlite3_module->xEof = nullptr;
//        _pSqlite3_module->xColumn = nullptr;
//        _pSqlite3_module->xRowid = nullptr;
//        _pSqlite3_module->xUpdate = nullptr;
//        _pSqlite3_module->xBegin = nullptr;
//        _pSqlite3_module->xSync = nullptr;
//        _pSqlite3_module->xCommit = nullptr;
//        _pSqlite3_module->xRollback = nullptr;
//        _pSqlite3_module->xFindFunction = nullptr;
//        _pSqlite3_module->xRename = nullptr;
//        _pSqlite3_module->xSavepoint = nullptr;
//        _pSqlite3_module->xRelease = nullptr;
//        _pSqlite3_module->xRollbackTo = nullptr;
	}

	static int xCreate(sqlite3* db, void *pAux,
		int argc, const char *const*argv,
		sqlite3_vtab **ppVTab, char** pzErr)
	{
		if (_instance->_createCallBack)
		{
			return _instance->_createCallBack(db, pAux,
				argc, argv,
				ppVTab, pzErr);
		}
		return -1;
	}
	static int xConnect(sqlite3* db, void *pAux,
		int argc, const char *const*argv,
		sqlite3_vtab **ppVTab, char** pzErr)
	{
		if (_instance->_connectCallBack)
		{
			return _instance->_connectCallBack(db, pAux, argc, argv, ppVTab, pzErr);
		}
		return -1;
	}
	static int xBestIndex(sqlite3_vtab *pVTab, sqlite3_index_info* pIdxInfo)
	{
		if (_instance->_bestIndexCallBack)
		{
			return _instance->_bestIndexCallBack(pVTab, pIdxInfo);
		}
		return -1;
	}
	static int xDisconnect(sqlite3_vtab *pVTab)
	{
		if (_instance->_disconnectCallBack)
		{
			return _instance->_disconnectCallBack(pVTab);
		}
		return -1;
	}
	static int xDestroy(sqlite3_vtab *pVTab)
	{
		if (_instance->_destroyCallBack)
		{
			return _instance->_destroyCallBack(pVTab);
		}
		return -1;
	}
	static int xOpen(sqlite3_vtab *pVTab, sqlite3_vtab_cursor **ppCursor)
	{
		if (_instance->_openCallBack)
		{
			return _instance->_openCallBack(pVTab, ppCursor);
		}
		return -1;
	}
	static int xClose(sqlite3_vtab_cursor* pcursor)
	{
		if (_instance->_closeCallBack)
		{
			return _instance->_closeCallBack(pcursor);
		}
		return -1;
	}
	static int xFilter(sqlite3_vtab_cursor* pcursor, int idxNum, const char *idxStr,
		int argc, sqlite3_value **argv)
	{
		if (_instance->_filterCallBack)
		{
			return _instance->_filterCallBack(pcursor, idxNum, idxStr, argc, argv);
		}
		return -1;
	}
	static int xNext(sqlite3_vtab_cursor* pcursor)
	{
		if (_instance->_nextCallBack)
		{
			return _instance->_nextCallBack(pcursor);
		}
		return -1;
	}
	static int xEof(sqlite3_vtab_cursor* pcursor)
	{
		if (_instance->_eofCallBack)
		{
			return _instance->_eofCallBack(pcursor);
		}
		return -1;
	}
	static int xColumn(sqlite3_vtab_cursor* pcursor, sqlite3_context* context, int i)
	{
		if (_instance->_columnCallBack)
		{
			return _instance->_columnCallBack(pcursor, context, i);
		}
		return -1;
	}
	static int xRowid(sqlite3_vtab_cursor* pcursor, sqlite3_int64 *pRowid)
	{
		if (_instance->_rowidCallBack)
		{
			return _instance->_rowidCallBack(pcursor, pRowid);
		}
		return -1;
	}
	static int xUpdate(sqlite3_vtab *pvtab, int, sqlite3_value ** ppsqlite3Value, sqlite3_int64 *psqlite364)
	{
		if (_instance->_updateCallBack)
		{
			return _instance->_updateCallBack(pvtab, ppsqlite3Value, psqlite364);
		}
		return -1;
	}
	static int xBegin(sqlite3_vtab *pVTab)
	{
		if (_instance->_beginCallBack)
		{
			return _instance->_beginCallBack(pVTab);
		}
		return -1;
	}
	static int xSync(sqlite3_vtab *pVTab)
	{
		if (_instance->_syncCallBack)
		{
			return _instance->_syncCallBack(pVTab);
		}
		return -1;
	}
	static int xCommit(sqlite3_vtab *pVTab)
	{
		if (_instance->_commitCallBack)
		{
			return _instance->_commitCallBack(pVTab);
		}
		return -1;
	}
	static int xRollback(sqlite3_vtab *pVTab)
	{
		if (_instance->_rollbackCallBack)
		{
			return _instance->_rollbackCallBack(pVTab);
		}
		return -1;
	}
	static int xFindFunction(sqlite3_vtab *pVtab, int nArg, const char *zName,
		void(**pxFunc)(sqlite3_context*, int, sqlite3_value**),
		void **ppArg)
	{
		if (_instance->_findFunctionCallBack)
		{
			return _instance->_findFunctionCallBack(pVtab, nArg, zName, pxFunc, ppArg);
		}
		return -1;
	}
	static int xRename(sqlite3_vtab *pVtab, const char *zNew)
	{
		if (_instance->_renameCallBack)
		{
			return _instance->_renameCallBack(pVtab, zNew);
		}
		return -1;
	}
	static int xSavepoint(sqlite3_vtab *pVTab, int i)
	{
		if (_instance->_savepointCallBack)
		{
			return _instance->_savepointCallBack(pVTab, i);
		}
		return -1;
	}
	static int xRelease(sqlite3_vtab *pVTab, int i)
	{
		if (_instance->_releaseCallBack)
		{
			return _instance->_releaseCallBack(pVTab, i);
		}
		return -1;
	}
	static int xRollbackTo(sqlite3_vtab *pVTab, int i)
	{
		if (_instance->_rollbackToCallBack)
		{
			return _instance->_rollbackToCallBack(pVTab, i);
		}
		return -1;
	}

    bool _startUped;
    sqlite3_module _sqlite3_module;
    sqlite3 *_dbPtr;
    int _version;
    CreateCallBack _createCallBack;
    ConnectCallBack _connectCallBack;
	BestIndexCallBack _bestIndexCallBack;
	DisconnectCallBack _disconnectCallBack;
	DestroyCallBack _destroyCallBack;
	OpenCallBack	_openCallBack;
	CloseCallBack _closeCallBack;
	FilterCallBack _filterCallBack;
	NextCallBack _nextCallBack;
	EofCallBack _eofCallBack;
	ColumnCallBack _columnCallBack;
	RowidCallBack _rowidCallBack;
	UpdateCallBack _updateCallBack;
	BeginCallBack _beginCallBack;
	SyncCallBack _syncCallBack;
	CommitCallBack _commitCallBack;
	RollbackCallBack _rollbackCallBack;
	FindFunctionCallBack _findFunctionCallBack;
	RenameCallBack _renameCallBack;
	SavepointCallBack _savepointCallBack;
	ReleaseCallBack _releaseCallBack;
	RollbackToCallBack _rollbackToCallBack;
	static std::shared_ptr<SQLiteParserAdapter> _instance;
};
template <char const* moduleName>
std::shared_ptr<SQLiteParserAdapter<moduleName>> SQLiteParserAdapter<moduleName>::_instance;
#endif // !SQLiteParserAdapter_H

