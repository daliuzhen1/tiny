#ifndef CSVSQLPARSER_H
#define CSVSQLPARSER_H
#include <string>
#include "include/file_extractor/CsvExtractor.h"
#include "sqlite3.h"
class CsvSQLParser
{
public:
    static std::shared_ptr<CsvSQLParser> getInstance();
    void initilize();
    sqlite3* parse(std::string& filePath);

private:
    CsvSQLParser();
    static char csv[];
    static std::shared_ptr<CsvSQLParser> _instance;
    bool initilized;
    std::map<std::string, std::string> _fileMap;
};

#endif // CSVSQLPARSER_H
