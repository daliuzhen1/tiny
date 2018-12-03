//
// CsvExtractor.h
//
// $Id: /FileExtractor.h#1 $
//
// Definition of the CsvExtractor class.
//
// Copyright (c) 
//


#ifndef CsvExtractor_INCLUDED
#define CsvExtractor_INCLUDED

#include <string>
#include <vector>
#include <memory>

#include <QFile>
#include <QDateTime>

#include "include/common/MetaData.h"
#include "include/Extractor.h"
class CsvExtractor : public Extractor
{
public:

    CsvExtractor(std::string& filePath);
    virtual ~CsvExtractor(void);

    bool parseData(outdata_t &vvs, bool &bLastPack);
    bool selectTable(std::string& name);
    std::shared_ptr<std::vector<std::string>> getTableList();
    std::string& getDatabaseName();
private:
    bool parseMetaData();
    bool parseDataType();
    char getDelimiter();
    bool isDateTime(const std::string &str, std::string& format, bool hasFormat);
    void initCurrencySymbol();
    bool hasCurrencySymbol(std::string& str);
    void disposeSpecialSE(std::string& str, bool &bDate);
    bool isThousandSep(const char c);
    bool isDecimalSep(const char c);
    DataType getFieldType(std::string& str, std::string &strDateFmt, char &guessSep, char &thSep, char &cDeSep);
private:

	std::string _validDelimiter;
    char _validDelimiterChar;
    std::string _filePath;
    std::shared_ptr<QFile> _pQfile;
    std::vector<std::string> s_vecCurrecny;
    std::string _databaseName;
};


#endif

