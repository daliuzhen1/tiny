#ifndef DBInterface_H
#define DBInterface_H

#include <memory>
#include <leveldb/db.h>
#include <QSqlDatabase>
#include <map>
class DBInterface
{

public:
   static std::shared_ptr<DBInterface> getDBInstance();
   QSqlDatabase& getSqlDB();
   std::map<std::string, std::shared_ptr<leveldb::DB>>  getNoSqlDB();

private:
   void initializeSqlDB();
   void initalizeNoSqlDB();
   DBInterface();
   static std::shared_ptr<DBInterface> _instance;
   QSqlDatabase _sqldb;
   std::map<std::string, std::shared_ptr<leveldb::DB>> _noSqldb;
};


#endif
