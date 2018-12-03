#include <QFileInfo>
#include <QDebug>
#include <QQuickItem>
#include <QSharedPointer>
#include <QUuid>
#include "include/view/DataViewQML.h"
#include "include/file_extractor/CsvExtractor.h"
#include "include/common/MetaData.h"
DataViewQML::DataViewQML(QObject *parent): QObject(parent),pDataTableModeForAllData(new DataTableModel),
    _isAllData(false),
    _currentDataBaseIndex(0)
{

}


QList<QString> DataViewQML::loadHistoryDatabase()
{
    std::vector<std::shared_ptr<SourceInfo>> sourceInfo = SourceInfo::fromDB();
    QList<QString> dataBaseName;

    for (int i = 0; i < sourceInfo.size(); i++)
    {
        std::shared_ptr<Extractor> _pCsvExtractor;
        if (sourceInfo.at(i)->sourceType == ST_CSV)
        {
            std::shared_ptr<CSVSourceInfo> csvSourceInfo = std::dynamic_pointer_cast<CSVSourceInfo>(sourceInfo.at(i));
            _pCsvExtractor = std::shared_ptr<CsvExtractor>(new CsvExtractor(csvSourceInfo->sourcePath));
        }
        extractorVec.push_back(_pCsvExtractor);
        dataBaseName.push_back(QString(sourceInfo.at(i)->sourceName.c_str()));
    }
    _sourceInfoVec = sourceInfo;
    return dataBaseName;
}

bool DataViewQML::uploadFile(QUrl filePath)
{
    if (!validFilePath(filePath))
        return false;
    QString extName =  QFileInfo(filePath.fileName()).suffix();
    std::shared_ptr<Extractor> _pCsvExtractor;
    if (extName == "csv")
    {
        std::string filePathStd = filePath.toLocalFile().toStdString();
        _pCsvExtractor = std::shared_ptr<CsvExtractor>(new CsvExtractor(filePathStd));
        std::shared_ptr<CSVSourceInfo>  pSourceInfo(new CSVSourceInfo);

        pSourceInfo->sourcePath = filePathStd;
        pSourceInfo->sourceID = QUuid::createUuid().toString().toStdString();
        pSourceInfo->sourceName = _pCsvExtractor->getDatabaseName();
        pSourceInfo->sourceType = ST_CSV;
        pSourceInfo->sourceTypeName = SourceInfo::sourceTypeToString(ST_CSV);
        if (!SourceInfo::toDB(pSourceInfo))
        {
            return false;
        }
        _sourceInfoVec.push_back(pSourceInfo);
    }
    extractorVec.push_back(_pCsvExtractor);
    return true;
}

Q_INVOKABLE QList<QString> DataViewQML::getTableList(int databaseIndex)
{
    QList<QString> qTabListStr;
    std::shared_ptr<std::vector<std::string>> pVecStr = extractorVec.at(databaseIndex)->getTableList();
    for (unsigned int i = 0; i < pVecStr->size(); i++)
        qTabListStr.push_back(QString(pVecStr->at(i).c_str()));
    return qTabListStr;
}

Q_INVOKABLE QString DataViewQML::getDataBaseName(int databaseIndex)
{
    return QString(extractorVec.at(databaseIndex)->getDatabaseName().c_str());
}


Q_INVOKABLE bool DataViewQML::deleteDataBase(int databaseIndex)
{
    std::vector<std::shared_ptr<Extractor>>::iterator itExt = extractorVec.begin();
    std::vector<std::shared_ptr<SourceInfo>>::iterator itSou = _sourceInfoVec.begin();
    if (databaseIndex < 0)
        return false;
    int idx = 0;
    while (idx < databaseIndex)
    {
        itExt++;
        itSou++;
    }
    extractorVec.erase(itExt);
    if (SourceInfo::removeFromDB((*itSou)->sourceID))
    {
        _sourceInfoVec.erase(itSou);
        return true;
    }
    return false;
}

bool DataViewQML::validFilePath(QUrl& filePath)
{
    if (!filePath.isValid())
        return false;
    QString extName =  QFileInfo(filePath.fileName()).suffix();
    if (extName == "csv")
    {
        return true;
    }
    return false;
}

bool DataViewQML::selectTable(int databaseIndex, QString tableName)
{
    bool finded = false;
    QList<QString> qTabListStr = getTableList(databaseIndex);
    for (unsigned int i = 0; i < qTabListStr.size(); i++)
    {
        if (tableName == qTabListStr[i])
        {
            finded = true;
            break;
        }
    }
    if (finded)
    {
        std::string tabNameStd = tableName.toStdString();
        if ( extractorVec.at(databaseIndex)->selectTable(tabNameStd))
        {
            _currentDataBaseIndex = databaseIndex;
            _currentTableName = QString(tabNameStd.c_str());
            return true;
        }
    }

    return false;
}


QVariant DataViewQML::getMetaData()
{
    std::vector<TableColumnInfo>& metadata = extractorVec.at(_currentDataBaseIndex)->getMetaData();
    _pTableColumnInfoQMLVec.clear();
    QVariantList qList;
    for (unsigned int i = 0; i < metadata.size(); i++)
    {
        std::shared_ptr<TableColumnInfoQML> pTableColumnInfoQML = TableColumnInfoQML::covertFromCPP(metadata[i]);
        // save the ptr
        _pTableColumnInfoQMLVec.push_back(pTableColumnInfoQML);
        qList.append((QVariant::fromValue<TableColumnInfoQML*>(pTableColumnInfoQML.get())));
    }
    return QVariant::fromValue(qList);
}


QVariant DataViewQML::getPreViewData()
{
    outdata_t& previewData = extractorVec.at(_currentDataBaseIndex)->getPreviewData();
    QSharedPointer<DataTableModel> pDataTableMode(new DataTableModel);
    pDataTableMode->setTableCoulumnInfos(_pTableColumnInfoQMLVec);
    pDataTableMode->setWithOutData(previewData);
    return QVariant::fromValue(pDataTableMode);
}

bool DataViewQML::createQuery()
{
    return true;
}

//QVariant DataViewQML::getAllData(bool firstPack)
//{
//    outdata_t outData;
//    if (firstPack)
//    {
//        pDataTableModeForAllData->setTableCoulumnInfos(_pTableColumnInfoQMLVec);
//    }
//    bool lastPack;
//    if (_pCsvExtractor->parseData(outData,lastPack))
//    {
//        _isAllData = lastPack;
//        qDebug() << "appendDataWithOutData";
//        pDataTableModeForAllData->appendDataWithOutData(outData);
//    }

//    return QVariant::fromValue(pDataTableModeForAllData);
//}


bool DataViewQML::finishingGetData()
{
    return _isAllData;
}
