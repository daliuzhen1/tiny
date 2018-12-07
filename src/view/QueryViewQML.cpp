#include <QUuid>
#include <QCoreApplication>
#include <QDir>
#include "include/view/QueryViewQML.h"
#include "include/column_storage/ColumnStorageWriter.h"
#include "include/column_storage/ColumnStorageReader.h"

QueryViewQML::QueryViewQML(QObject *parent, QString queryID)
{

}

QueryViewQML::QueryViewQML(QObject *parent)
{

}
QueryViewQML::QueryViewQML(QObject *parent, std::string& queryName, std::shared_ptr<SourceInfo> pSourceInfo, std::shared_ptr<Extractor> pExtractor) : QObject (parent)
{
    _pQueryInfo = std::shared_ptr<QueryInfo>(new QueryInfo);
    _pQueryInfo->pSourceInfo = pSourceInfo;
    _pExtractoVec.push_back(pExtractor);
    _pQueryInfo->queryName = queryName;
    _pQueryInfo->queryID = QUuid::createUuid().toString().toStdString();
    _pQueryInfo->nodeNum = 1;
    std::shared_ptr<QueryNode> pQueryNode(new QueryNode);
    pQueryNode->nodeIndex = 0;
    pQueryNode->nodeName = queryName;
    pQueryNode->queryID = _pQueryInfo->queryID;
    pQueryNode->operationOnNode = "";
    // copy a new
    std::vector<TableColumnInfo> &tableColumnInfoVec  = pExtractor->getMetaData();
    for (int i = 0; i < tableColumnInfoVec.size(); i++)
    {
        std::shared_ptr<TableColumnInfo> pTableColumnInfo(new TableColumnInfo(tableColumnInfoVec.at(i)));
        pQueryNode->pTableColumnInfo.push_back(pTableColumnInfo);
    }
    pQueryNode->nodeID = pQueryNode->queryID + QString::number(pQueryNode->nodeIndex).toStdString();
    _pQueryInfo->pQueryNodeVec.push_back(pQueryNode);
    if (!QueryInfo::toDB(_pQueryInfo))
        throw std::runtime_error("create query error");
}

Q_INVOKABLE QVariant QueryViewQML::getMetaData(int nodeIndex)
{
    if (nodeIndex >= _pQueryInfo->nodeNum)
        return QVariant();
    pTableColumnInfoQMLVec.clear();
    std::vector<TableColumnInfo::Ptr> pMetadata = _pQueryInfo->pQueryNodeVec.at(nodeIndex)->pTableColumnInfo;
    QVariantList qList;
    for (unsigned int i = 0; i < pMetadata.size(); i++)
    {
        std::shared_ptr<TableColumnInfoQML> pTableColumnInfoQML = TableColumnInfoQML::covertFromCPP(*(pMetadata.at(i)));
        // save the ptr
        pTableColumnInfoQMLVec.push_back(pTableColumnInfoQML);
        qList.append((QVariant::fromValue<TableColumnInfoQML*>(pTableColumnInfoQML.get())));
    }
    return QVariant::fromValue(qList);
}

QVariant QueryViewQML::getData(int index)
{
    outdata_t vvs;
    bool bLastPack = false;;

    QSharedPointer<DataTableModel> pDataTableMode(new DataTableModel);
    std::vector<TableColumnInfo::Ptr> pMetadata = _pQueryInfo->pQueryNodeVec.at(index)->pTableColumnInfo;

    std::vector<std::shared_ptr<TableColumnInfoQML>> pTableColumnInfoQMLVec;
    for (unsigned int i = 0; i < pMetadata.size(); i++)
    {
        std::shared_ptr<TableColumnInfoQML> pTableColumnInfoQML = TableColumnInfoQML::covertFromCPP(*(pMetadata.at(i)));
        pTableColumnInfoQMLVec.push_back(pTableColumnInfoQML);
    }
    pDataTableMode->setTableCoulumnInfos(pTableColumnInfoQMLVec);
    QString path = QCoreApplication::applicationDirPath();
    path += QDir::separator();
    path += _pQueryInfo->pQueryNodeVec.at(index)->nodeID.c_str();
    path += ".parquet";
    std::string strPath = path.toStdString();
    ColumnStorageWriter columnStorageWriter(pMetadata, strPath);
    while(!bLastPack)
    {
        if (!_pExtractoVec.at(index)->parseData(vvs, bLastPack))
            return QVariant();
        columnStorageWriter.write_batch(vvs, bLastPack);
    }
    bLastPack = false;

    ColumnStorageReader columnStorageReader(pMetadata, strPath);
    while (!bLastPack)
    {
        outdata_t vvs;
        columnStorageReader.read_batch(vvs, bLastPack);
        pDataTableMode->appendDataWithOutData(vvs);
    }
    qDebug() << pDataTableMode->rowCount();
    return QVariant::fromValue(pDataTableMode);
}

