#include "include/common/DBInterface.h"
#include <QSqlDatabase>
#include <QSqlQuery>

std::shared_ptr<DBInterface> DBInterface::_instance;
std::shared_ptr<DBInterface> DBInterface::getDBInstance()
{
    if (!_instance)
        _instance = std::shared_ptr<DBInterface>(new DBInterface());
    return _instance;
}

DBInterface::DBInterface()
{
    initializeDB();
}

void DBInterface::initializeDB()
{
    _sqldb = QSqlDatabase::addDatabase("QSQLITE", "sqlite");
    _sqldb.setDatabaseName("meta.db");
    if(!_sqldb.open())
    {
        throw std::runtime_error("init db error");
    }

    QSqlQuery query(_sqldb);
    bool success = query.exec("create table if not exists query(queryID varchar primary key,queryName varchar,"
                                  "sourceID varchar, nodeNum int)");
    if(!success)
    {
        throw std::runtime_error("init db error");
    }
    success = query.exec("create table if not exists source(sourceID varchar primary key,sourceType int, "
                         "sourceName varchar, sourceDetail varchar)");
    if(!success)
    {
        throw std::runtime_error("init db error");
    }

    success = query.exec("create table if not exists queryNode(nodeID varchar primary key,queryID varchar, nodeIndex int, "
                         "operationOnNode varchar, columnInfosID)");
    if(!success)
    {
        throw std::runtime_error("init db error");
    }

//    leveldb::DB* leveldb;
//    leveldb::Options options;
//    options.create_if_missing = true;
//    std::shared_ptr<leveldb::DB> pLevelDB;
//    leveldb::Status status = leveldb::DB::Open(options, "columnInfos", &leveldb);
//    std::shared_ptr<leveldb::DB> levelDbptr(leveldb);
//    _noSqldb["columnInfos"] = levelDbptr;
//    if (false == status.ok())
//    {
//        throw std::runtime_error("init db error");
//    }
}


QSqlDatabase& DBInterface::getSqlDB()
{
    return _sqldb;
}


std::map<std::string, std::shared_ptr<leveldb::DB>> DBInterface::getNoSqlDB()
{
    return _noSqldb;
}
