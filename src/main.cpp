#include <QGuiApplication>
#include <QQmlApplicationEngine>


#include <QDate>
#include <iostream>
#include <sstream>
#include <string>
#include "include/common/DBInterface.h"
#include "include/view/DataViewQML.h"
#include "include/view/DataTableModel.h"
#include "include/view/MetaDataQML.h"
#include "include/sql_parser/CsvSQLParser.h"
#include "third-party/sqlite3/include/sqlite3.h"
using namespace std;
int callback_selectAll(void * params, int n_clumn, char **column_value, char ** column_name)
{
    int i = 0;

    // 打印表中的值
    for (; i<n_clumn; i++)
    {
        qDebug() << column_value[i];
    }

    return 0;
}
int main(int argc, char *argv[])
{
    DBInterface::getDBInstance();
//     Set up database connection information and open database

//    leveldb::Status status = leveldb::DB::Open(options, "./testdb", &db);

//    if (false == status.ok())
//    {
//        return -1;
//    }

    // Add 256 values to the database
//    leveldb::WriteOptions writeOptions;
//     db->Put(writeOptions, "aaa", "bbb");
//    for (unsigned int i = 0; i < 256; ++i)
//    {
//        std::ostringstream keyStream;
//        keyStream << "Key" << i;

//        std::ostringstream valueStream;
//        valueStream << "Test data value: " << i;

//        db->Put(writeOptions, "aaa", "bbb");
//    }

//    // Iterate over each item in the database and print them
//    leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());

//    for (it->SeekToFirst(); it->Valid(); it->Next())
//    {
//        cout << it->key().ToString() << " : " << it->value().ToString() << endl;
//    }

//    if (false == it->status().ok())
//    {
//        cerr << "An error was found during the scan" << endl;
//        cerr << it->status().ToString() << endl;
//    }
//    string value;
////    db->Get(leveldb::ReadOptions(), "aaa", &value);
//    qDebug()<< value.c_str();
//    delete it;

    // Close the database
//    delete db;


//    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);



//    std::string aaa("C:\\Users\\zhenl\\yahoo_prices.csv");
//    std::string bbb("C:\\Users\\zhenl\\yahoo_prices1.csv");
//    CsvSQLParser::getInstance()->initilize();
//    sqlite3* dblite = CsvSQLParser::getInstance()->parse(aaa);
//    CsvSQLParser::getInstance()->parse(bbb);
//    char *error;
//    const char *sel =  "select yahoo_prices1.Open1 from yahoo_prices join yahoo_prices1 on  yahoo_prices.Date = yahoo_prices1.Date";
//    int rc = sqlite3_exec(dblite, sel, callback_selectAll, 0, &error);
//    if (rc != SQLITE_OK ) {
//         printf("SQL error: %s\n", error);
//         sqlite3_free(nullptr);
//    }
    QGuiApplication::setAttribute(Qt::AA_UseOpenGLES, true);

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    qmlRegisterType<DataViewQML>("AllIn", 1, 0, "DataViewQML");
    qmlRegisterType<TableColumnInfoQML>("AllIn", 1, 0, "TableColumnInfoQML");
    qmlRegisterType<DataTableModel>("AllIn", 1, 0, "DataTableModel");
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
