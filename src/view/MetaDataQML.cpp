#include "include/view/MetaDataQML.h"
TableColumnInfoQML::TableColumnInfoQML(QObject *parent): QObject(parent)
{

}


std::shared_ptr<TableColumnInfo> TableColumnInfoQML::covertFromQML(TableColumnInfoQML& tableColumnInfoQML)
{
    std::shared_ptr<TableColumnInfo> pTableColumnInfo = std::shared_ptr<TableColumnInfo>(new TableColumnInfo);
    pTableColumnInfo->colID = tableColumnInfoQML.colID().toStdString();
    pTableColumnInfo->colName = tableColumnInfoQML.colName().toStdString();
    pTableColumnInfo->colFormat = tableColumnInfoQML.colFormat().toStdString();
    pTableColumnInfo->colType = (DataType)tableColumnInfoQML.colType();
    pTableColumnInfo->colIndex = tableColumnInfoQML.colIndex();
    pTableColumnInfo->used = tableColumnInfoQML.used();
    return pTableColumnInfo;
}

std::shared_ptr<TableColumnInfoQML> TableColumnInfoQML::covertFromCPP(TableColumnInfo& tableColumnInfo)
{
    std::shared_ptr<TableColumnInfoQML> pTableColumnInfoQML = std::shared_ptr<TableColumnInfoQML>(new TableColumnInfoQML);
    pTableColumnInfoQML->setColID(tableColumnInfo.colID.c_str());
    pTableColumnInfoQML->setColIndex(tableColumnInfo.colIndex);
    pTableColumnInfoQML->setColName(tableColumnInfo.colName.c_str());
    pTableColumnInfoQML->setColFormat(tableColumnInfo.colFormat.c_str());
    TableColumnInfoQML::DataTypeQML dtq = (TableColumnInfoQML::DataTypeQML)(tableColumnInfo.colType);
    pTableColumnInfoQML->setColType(dtq);
    pTableColumnInfoQML->setUsed(tableColumnInfo.used);
    return pTableColumnInfoQML;
}


std::shared_ptr<QueryNode> QueryNodeQML::covertFromQML(QueryNodeQML& queryNodeQML)
{
    std::shared_ptr<QueryNode> queryNode(new QueryNode);
    queryNode->nodeID = queryNodeQML.nodeID().toStdString();
    queryNode->nodeIndex = queryNodeQML.nodeIndex();
    queryNode->nodeName = queryNodeQML.nodeName().toStdString();
    queryNode->queryID = queryNodeQML.queryID().toStdString();
    queryNode->operationOnNode = queryNodeQML.operationOnNode().toStdString();
    queryNode->pTableColumnInfo = queryNodeQML.getTableColumnInfo();
    return queryNode;

}
std::shared_ptr<QueryNodeQML> QueryNodeQML::covertFromCPP(QueryNode& queryNode)
{
    std::shared_ptr<QueryNodeQML> queryNodeQML(new QueryNodeQML);
    queryNodeQML->setNodeID(queryNode.nodeID.c_str());
    queryNodeQML->setNodeName(queryNode.nodeName.c_str());
    queryNodeQML->setNodeIndex(queryNode.nodeIndex);
    queryNodeQML->setQueryID(queryNode.queryID.c_str());
    queryNodeQML->setOperationOnNode(queryNode.operationOnNode.c_str());
    queryNodeQML->setTableColumnInfo(queryNode.pTableColumnInfo);
    return queryNodeQML;
}


std::vector<TableColumnInfo::Ptr>& QueryNodeQML::getTableColumnInfo()
{
    return _pTableColumnInfo;
}

void QueryNodeQML::setTableColumnInfo(std::vector<TableColumnInfo::Ptr>& pTableColumnInfo)
{
    _pTableColumnInfo = pTableColumnInfo;
}
