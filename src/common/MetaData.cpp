#include "include/common/MetaData.h"
#include "include/common/DBInterface.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlRecord>

std::string SourceInfo::sourceTypeToString(SourceType sourceType)
{
    switch (sourceType)
    {
    case ST_CSV:
    {
        return "csv";
    }
    }
}
std::shared_ptr<SourceInfo> SourceInfo::fromDBWithSourceID(std::string& sourceID)
{
    QSqlDatabase& sqlDB = DBInterface::getDBInstance()->getSqlDB();
    QSqlQuery query(sqlDB);
    query.prepare("select * from source where sourceID = :sourceID");
    query.bindValue(":sourceID", sourceID.c_str());
    query.exec();
    if (query.record().count() != 1)
    {
        return nullptr;
    }

    if (query.next())
    {
        std::shared_ptr<SourceInfo> pSourceInfo;
        if (query.value("sourceType").toInt() == ST_CSV)
        {
            pSourceInfo = std::shared_ptr<CSVSourceInfo>(new CSVSourceInfo);
        }
        pSourceInfo->sourceType = static_cast<SourceType>(query.value("sourceType").toInt());
        pSourceInfo->sourceTypeName = sourceTypeToString(pSourceInfo->sourceType);
        pSourceInfo->sourceName = query.value("sourceName").toString().toStdString();
        pSourceInfo->sourceID = sourceID;
        std::string sourceDetail = query.value("sourceDetail").toString().toStdString();
        pSourceInfo->unmakeDetail(sourceDetail);
        return pSourceInfo;
    }
    return nullptr;

}
std::vector<std::shared_ptr<SourceInfo>> SourceInfo::fromDBWithSourceName(std::string& sourceName)
{
    QSqlDatabase& sqlDB = DBInterface::getDBInstance()->getSqlDB();
    QSqlQuery query(sqlDB);
    QString likeStr = QString().sprintf("'%%%s%%'", sourceName.c_str());
    QString statement = "select * from source where sourceName like ";
    statement += likeStr;
    query.prepare(statement);
    query.exec();
    std::vector<std::shared_ptr<SourceInfo>> ret;
    if (query.record().isEmpty())
    {
        return ret;
    }
    while (query.next())
    {
        std::shared_ptr<SourceInfo> pSourceInfo;
        if (query.value("sourceType").toInt() == ST_CSV)
        {
            pSourceInfo = std::shared_ptr<CSVSourceInfo>(new CSVSourceInfo);
        }
        pSourceInfo->sourceType = static_cast<SourceType>(query.value("sourceType").toInt());
        pSourceInfo->sourceTypeName = sourceTypeToString(pSourceInfo->sourceType);
        pSourceInfo->sourceName = query.value("sourceName").toString().toStdString();
        pSourceInfo->sourceID = query.value("sourceID").toString().toStdString();
        std::string sourceDetail = query.value("sourceDetail").toString().toStdString();
        pSourceInfo->unmakeDetail(sourceDetail);
        ret.push_back(pSourceInfo);
    }
    return ret;
}

std::vector<std::shared_ptr<SourceInfo>> SourceInfo::fromDBWithSourceType(SourceType& sourceType)
{
    QSqlDatabase& sqlDB = DBInterface::getDBInstance()->getSqlDB();
    QSqlQuery query(sqlDB);
    query.prepare("select * from source where sourceType = :sourceType");
    query.bindValue(":sourceType", sourceType);
    query.exec();
    std::vector<std::shared_ptr<SourceInfo>> ret;
    if (query.record().isEmpty())
    {
        return ret;
    }
    while (query.next())
    {
        std::shared_ptr<SourceInfo> pSourceInfo;
        if (query.value("sourceType").toInt() == ST_CSV)
        {
            pSourceInfo = std::shared_ptr<CSVSourceInfo>(new CSVSourceInfo);
        }
        pSourceInfo->sourceType = static_cast<SourceType>(query.value("sourceType").toInt());
        pSourceInfo->sourceTypeName = sourceTypeToString(pSourceInfo->sourceType);
        pSourceInfo->sourceName = query.value("sourceName").toString().toStdString();
        pSourceInfo->sourceID = query.value("sourceID").toString().toStdString();
        std::string sourceDetail = query.value("sourceDetail").toString().toStdString();
        pSourceInfo->unmakeDetail(sourceDetail);
        ret.push_back(pSourceInfo);
    }
    return ret;
}


std::vector<std::shared_ptr<SourceInfo>> SourceInfo::fromDB()
{
    QSqlDatabase& sqlDB = DBInterface::getDBInstance()->getSqlDB();
    QSqlQuery query(sqlDB);
    query.prepare("select * from source");
    query.exec();
    std::vector<std::shared_ptr<SourceInfo>> ret;
    if (query.record().isEmpty())
    {
        return ret;
    }
    while (query.next())
    {
        std::shared_ptr<SourceInfo> pSourceInfo;
        if (query.value("sourceType").toInt() == ST_CSV)
        {
            pSourceInfo = std::shared_ptr<CSVSourceInfo>(new CSVSourceInfo);
        }
        pSourceInfo->sourceType = static_cast<SourceType>(query.value("sourceType").toInt());
        pSourceInfo->sourceTypeName = sourceTypeToString(pSourceInfo->sourceType);
        pSourceInfo->sourceName = query.value("sourceName").toString().toStdString();
        pSourceInfo->sourceID = query.value("sourceID").toString().toStdString();
        std::string sourceDetail = query.value("sourceDetail").toString().toStdString();
        pSourceInfo->unmakeDetail(sourceDetail);
        ret.push_back(pSourceInfo);
    }
    return ret;
}


bool SourceInfo::removeFromDB(std::string& sourceID)
{
    QSqlDatabase& sqlDB = DBInterface::getDBInstance()->getSqlDB();
    QSqlQuery query(sqlDB);
    query.prepare("delete from source where sourceID = :sourceID");
    query.bindValue(":sourceID", sourceID.c_str());
    if (query.exec())
    {
        return true;
    }
    return false;
}


bool SourceInfo::toDB(std::shared_ptr<SourceInfo> sourceInfo)
{
    if (!sourceInfo)
    {
        return false;
    }
    QSqlDatabase& sqlDB = DBInterface::getDBInstance()->getSqlDB();
    QSqlQuery query(sqlDB);
    query.prepare("insert into source (sourceID, sourceType, sourceName, sourceDetail) "
                  "VALUES (:sourceID, :sourceType, :sourceName, :sourceDetail)");
    query.bindValue(":sourceID", sourceInfo->sourceID.c_str());
    query.bindValue(":sourceType", sourceInfo->sourceType);
    query.bindValue(":sourceName", sourceInfo->sourceName.c_str());
    query.bindValue(":sourceDetail", sourceInfo->makeDetail().c_str());
    if (query.exec())
    {
        return true;
    }
    return false;
}


std::string CSVSourceInfo::makeDetail()
{
    QJsonObject qJsonObj;
    qJsonObj["sourcePath"] = sourcePath.c_str();
    QJsonDocument saveDoc(qJsonObj);
    return saveDoc.toJson().toStdString();
}


bool CSVSourceInfo::unmakeDetail(std::string& sourceDetail)
{
    QJsonObject qJsonObj = QJsonDocument::fromJson(sourceDetail.c_str()).object();
    if (qJsonObj.contains("sourcePath"))
        sourcePath = qJsonObj["sourcePath"].toString().toStdString();
    else
        return false;
    return true;
}


std::shared_ptr<QueryInfo> QueryInfo::fromDBWithQueryID(std::string& queryID)
{
    QSqlDatabase& sqlDB = DBInterface::getDBInstance()->getSqlDB();
    QSqlQuery query(sqlDB);
    query.prepare("select * from query where queryID = :queryID");
    query.bindValue(":queryID", queryID.c_str());
    query.exec();
    if (query.record().count() != 1)
    {
        return nullptr;
    }

    if (query.next())
    {
        std::shared_ptr<QueryInfo> pQueryInfo(new QueryInfo);
        pQueryInfo->queryID = queryID;
        pQueryInfo->queryName = query.value("queryName").toString().toStdString();
        pQueryInfo->nodeNum = query.value("nodeNum").toInt();
        std::string sourceID = query.value("sourceID").toString().toStdString();
        pQueryInfo->pSourceInfo  = SourceInfo::fromDBWithSourceID(sourceID);
        if (!pQueryInfo->pSourceInfo)
            return nullptr;
        return pQueryInfo;
    }
    return nullptr;
}
std::vector<std::shared_ptr<QueryInfo>> QueryInfo::fromDBWithQueryName(std::string& queryName)
{
    QSqlDatabase& sqlDB = DBInterface::getDBInstance()->getSqlDB();
    QSqlQuery query(sqlDB);
    QString likeStr = QString().sprintf("'%%%s%%'", queryName.c_str());
    QString statement = "select * from query where queryName like ";
    statement += likeStr;
    query.prepare(statement);
    query.exec();
    std::vector<std::shared_ptr<QueryInfo>> queryInfos;
    if (query.record().isEmpty())
    {
        return queryInfos;
    }
    while (query.next())
    {
        std::shared_ptr<QueryInfo> pQueryInfo(new QueryInfo);
        pQueryInfo->queryID = query.value("queryID").toString().toStdString();
        pQueryInfo->queryName = query.value("queryName").toString().toStdString();
        pQueryInfo->nodeNum = query.value("nodeNum").toInt();
        std::string sourceID = query.value("sourceID").toString().toStdString();
        pQueryInfo->pSourceInfo  = SourceInfo::fromDBWithSourceID(sourceID);
        if (!pQueryInfo->pSourceInfo)
        {
            queryInfos.clear();
            return queryInfos;
        }
        queryInfos.push_back(pQueryInfo);
    }
    return queryInfos;

}
std::vector<std::shared_ptr<QueryInfo>> QueryInfo::fromDBWithSourceID(std::string& sourceID)
{
    QSqlDatabase& sqlDB = DBInterface::getDBInstance()->getSqlDB();
    QSqlQuery query(sqlDB);
    query.prepare("select * from query where sourceID = :sourceID");
    query.bindValue(":sourceID", sourceID.c_str());
    query.exec();
    std::vector<std::shared_ptr<QueryInfo>> queryInfos;
    if (query.record().isEmpty())
    {
        return queryInfos;
    }
    while (query.next())
    {
        std::shared_ptr<QueryInfo> pQueryInfo(new QueryInfo);
        pQueryInfo->queryID = query.value("queryID").toString().toStdString();
        pQueryInfo->queryName = query.value("queryName").toString().toStdString();
        pQueryInfo->nodeNum = query.value("nodeNum").toInt();
        std::string sourceID = query.value("sourceID").toString().toStdString();
        pQueryInfo->pSourceInfo  = SourceInfo::fromDBWithSourceID(sourceID);
        if (!pQueryInfo->pSourceInfo)
        {
            queryInfos.clear();
            return queryInfos;
        }
        queryInfos.push_back(pQueryInfo);
    }
    return queryInfos;
}

bool QueryInfo::toDB(std::shared_ptr<QueryInfo> queryInfo)
{
    if (!queryInfo)
    {
        return false;
    }
    QSqlDatabase& sqlDB = DBInterface::getDBInstance()->getSqlDB();
    QSqlQuery query(sqlDB);
    query.prepare("insert into query (queryID, queryName, nodeNum, sourceID) "
                  "VALUES (:queryID, :queryName, :nodeNum, :sourceID)");
    query.bindValue(":queryID", queryInfo->queryID.c_str());
    query.bindValue(":queryName", queryInfo->queryName.c_str());
    query.bindValue(":nodeNum", queryInfo->nodeNum);
    query.bindValue(":sourceID", queryInfo->pSourceInfo->sourceID.c_str());
    if (query.exec())
    {
        return true;
    }
    return false;
}


std::vector<TableColumnInfo::Ptr> TableColumnInfo::fromDBWithNodeID(std::string& nodeID)
{
    std::vector<TableColumnInfo::Ptr> ret;
    std::shared_ptr<leveldb::DB> noSqlDb = DBInterface::getDBInstance()->getNoSqlDB()["columnInfos"];
    std::string jsonStr;
    leveldb::Status status = noSqlDb->Get(leveldb::ReadOptions(), nodeID, &jsonStr);
    if (false == status.ok())
    {
        return ret;
    }
    QJsonArray qJsonArray = QJsonDocument::fromJson(jsonStr.c_str()).array();
    for (unsigned int i = 0; i < qJsonArray.size(); i++)
    {
        QJsonObject qJsonObj = qJsonArray[i].toObject();
        TableColumnInfo::Ptr pTableColInfo(new TableColumnInfo);
        pTableColInfo->colFormat = qJsonObj["colFormat"].toString().toStdString();
        pTableColInfo->colIndex = qJsonObj["colIndex"].toInt();
        pTableColInfo->colName = qJsonObj["colName"].toString().toStdString();
        pTableColInfo->colType = static_cast<DataType>(qJsonObj["colType"].toInt());
        pTableColInfo->colID = qJsonObj["colID"].toString().toStdString();
        pTableColInfo->nodeID = qJsonObj["nodeID"].toString().toStdString();
        pTableColInfo->used = qJsonObj["used"].toBool();
        ret.push_back(pTableColInfo);
    }
    return ret;
}

bool TableColumnInfo::toDBWithNodeID(std::string& nodeID, std::vector<TableColumnInfo::Ptr> tableColInfos)
{
    std::shared_ptr<leveldb::DB> noSqlDb = DBInterface::getDBInstance()->getNoSqlDB()["columnInfos"];
    QJsonArray qJsonArray;
    for (unsigned int i = 0; i < tableColInfos.size(); i++)
    {
        QJsonObject qJsonObj;
        qJsonObj["colFormat"] = tableColInfos.at(i)->colFormat.c_str();
        qJsonObj["colIndex"] = tableColInfos.at(i)->colIndex;
        qJsonObj["colName"] = tableColInfos.at(i)->colName.c_str();
        qJsonObj["colType"] = tableColInfos.at(i)->colType;
        qJsonObj["colID"] = tableColInfos.at(i)->colID.c_str();
        qJsonObj["nodeID"] = tableColInfos.at(i)->nodeID.c_str();
        qJsonObj["used"] = tableColInfos.at(i)->used;
        qJsonArray.append(qJsonObj);
    }
    QJsonDocument saveDoc(qJsonArray);
    leveldb::WriteOptions writeOptions;
    leveldb::Status status = noSqlDb->Put(writeOptions, nodeID,  saveDoc.toJson().toStdString());
    if (false == status.ok())
    {
        return false;
    }
    return true;
}


std::shared_ptr<QueryNode> QueryNode::fromDBWithNodeID(std::string& nodeID)
{
    QSqlDatabase& sqlDB = DBInterface::getDBInstance()->getSqlDB();
    QSqlQuery query(sqlDB);
    query.prepare("select * from queryNode where nodeID = :nodeID");
    query.bindValue(":nodeID", nodeID.c_str());
    query.exec();
    if (query.record().count() != 1)
    {
        return nullptr;
    }
    if (query.next())
    {
        std::shared_ptr<QueryNode> pQueryNode(new QueryNode);
        pQueryNode->nodeID = nodeID;
        pQueryNode->nodeIndex = query.value("nodeIndex").toInt();
        pQueryNode->operationOnNode = query.value("operationOnNode").toString().toStdString();
        pQueryNode->queryID = query.value("queryID").toString().toStdString();
        return pQueryNode;
    }
    return nullptr;
}
std::vector<std::shared_ptr<QueryNode>> QueryNode::fromDBWithQueryID(std::string& queryID)
{
    QSqlDatabase& sqlDB = DBInterface::getDBInstance()->getSqlDB();
    QSqlQuery query(sqlDB);
    query.prepare("select * from queryNode where queryID = :queryID");
    query.bindValue(":queryID", queryID.c_str());
    query.exec();
    std::vector<std::shared_ptr<QueryNode>> queryNodes;
    if (query.record().isEmpty())
    {
        return queryNodes;
    }
    while (query.next())
    {
        std::shared_ptr<QueryNode> pQueryNode(new QueryNode);
        pQueryNode->nodeID = query.value("nodeID").toString().toStdString();
        pQueryNode->nodeIndex = query.value("nodeIndex").toInt();
        pQueryNode->operationOnNode = query.value("operationOnNode").toString().toStdString();
        pQueryNode->queryID = query.value("queryID").toString().toStdString();
        queryNodes.push_back(pQueryNode);
    }
    return queryNodes;
}
bool QueryNode::toDB(std::shared_ptr<QueryNode> queryNode)
{
    if (!queryNode)
    {
        return false;
    }
    QSqlDatabase& sqlDB = DBInterface::getDBInstance()->getSqlDB();
    QSqlQuery query(sqlDB);
    query.prepare("insert into queryNode (nodeID, queryID, nodeIndex, operationOnNode, columnInfosID) "
                  "VALUES (:nodeID, :queryID, :nodeIndex, :operationOnNode, :columnInfosID)");
    query.bindValue(":nodeID", queryNode->nodeID.c_str());
    query.bindValue(":queryID", queryNode->queryID.c_str());
    query.bindValue(":nodeIndex", queryNode->nodeIndex);
    query.bindValue(":operationOnNode", queryNode->operationOnNode.c_str());
    if (query.exec())
    {
        return true;
    }
    return false;
}

//std::string CSVSourceInfo::toJsonStr(Ptr csvInfoPtr)
//{
//    QJsonObject qJsonObj;
//    qJsonObj["sourcePath"] = csvInfoPtr->sourcePath.c_str();
//    qJsonObj["sourceID"] = csvInfoPtr->sourceID.c_str();
//    qJsonObj["sourceTypeName"] = csvInfoPtr->sourceTypeName.c_str();
//    qJsonObj["sourceType"] = sourceType;
//    QJsonDocument saveDoc(qJsonObj);
//    return saveDoc.toJson().toStdString();
//}

//Ptr CSVSourceInfo::fromJson(const std::string& jsonStr)
//{

//}
//std::string CSVSourceInfo::toJsonStr()
//{
//    QJsonObject qJsonObj;
//    qJsonObj["sourcePath"] = sourcePath.c_str();
//    qJsonObj["sourceID"] = sourceID.c_str();
//    qJsonObj["sourceTypeName"] = sourceTypeName.c_str();
//    qJsonObj["sourceType"] = sourceType;
//    QJsonDocument saveDoc(qJsonObj);
//    return saveDoc.toJson().toStdString();
//}

//SourceInfo::Ptr CSVSourceInfo::fromJson(const std::string& jsonStr)
//{
//    QJsonObject qJsonObj = QJsonDocument::fromJson(jsonStr.c_str()).object();
//    Ptr sourcePtr = new CSVSourceInfo();
//}
