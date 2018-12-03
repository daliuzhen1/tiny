#include <QByteArray>
#include <QDateTime>
#include <QString>
#include <QFileInfo>

#include "include/file_extractor/CsvExtractor.h"
#include "include/utility/CVDate.h"

#define MAXLINEFORDEL 100
#define PREVIEWCOUNT 100
#define PERREADCOUNT 100

using namespace std;
char const cThousandSep[] = {',', '.', ' ', '\''};
char const cDecimalSep[] = {',', '.'};
CsvExtractor::CsvExtractor(std::string& filePath):
    _filePath(filePath)
{
    _validDelimiter = std::string(",|;\t^~ "); // Supported filed delimiter
    _pQfile = std::shared_ptr<QFile>(new QFile(QString(_filePath.c_str())));
    if (!_pQfile->exists())
        throw runtime_error("the file is not exist");
    _databaseName = QFileInfo(_pQfile->fileName()).baseName().toStdString();
}

CsvExtractor::~CsvExtractor(void)
{
}


std::shared_ptr<std::vector<std::string>> CsvExtractor::getTableList()
{
    std::shared_ptr<std::vector<std::string>> ptabList = std::shared_ptr<std::vector<std::string>>(new std::vector<std::string>);
    ptabList->push_back(QFileInfo(_pQfile->fileName()).baseName().toStdString());
    return ptabList;
}


bool CsvExtractor::selectTable(std::string& name)
{
    _validDelimiterChar = getDelimiter();
    parseMetaData();
    return true;
}


std::string& CsvExtractor::getDatabaseName()
{
    return _databaseName;
}
char CsvExtractor::getDelimiter()
{
    if (!_pQfile->isOpen() && !_pQfile->open(QIODevice::ReadOnly)) {
        throw runtime_error("can not open the file");
    }
    qint64 pos = _pQfile->pos();
    int lineCount = 0;
    std::vector<int> delimiterCountInLine;
    char validDel = _validDelimiter[0];
    if (_pQfile->atEnd())
        throw runtime_error("it is a null file");
    struct Delimiter
    {
        int count;
        char delimiter;
        bool valided;
    };

    std::vector<Delimiter> delimiters;
    for (unsigned int i = 0; i < _validDelimiter.size(); i++)
    {
        Delimiter delimiter;
        delimiter.delimiter = _validDelimiter[i];
        delimiter.valided = true;
        delimiters.push_back(delimiter);
    }

    QByteArray line = _pQfile->readLine();
    int validCount = 0;
    for (unsigned int i = 0; i < delimiters.size(); i++)
    {
        int count = line.count(delimiters[i].delimiter);
        if (count)
        {
            validCount++;
            delimiters[i].valided = true;
            delimiters[i].count = count;
            validDel = delimiters[i].delimiter;
        }
        else
            delimiters[i].valided = false;
    }

    while (!_pQfile->atEnd() && lineCount < MAXLINEFORDEL && validCount > 1) {
        QByteArray line = _pQfile->readLine();
        validCount = 0;
        for (unsigned int i = 0; i < delimiters.size(); i++)
        {
            if (!delimiters[i].valided)
                continue;
            if (line.count(delimiters[i].delimiter) != delimiters[i].count)
                delimiters[i].valided = false;
        }
        for (unsigned int i = 0; i < delimiters.size(); i++)
        {

            if (delimiters[i].valided)
            {
                validDel = delimiters[i].delimiter;
                validCount++;
            }
        }
    }
    _pQfile->seek(pos);
    if (validCount == 0)
        validDel = _validDelimiter[0];
    return validDel;
}


bool CsvExtractor::parseMetaData()
{
    _previewData.clear();
    _metaData.clear();
    qint64 pos = _pQfile->pos();
    if (_pQfile->atEnd())
        return false;
    for (int i = 0; i < PREVIEWCOUNT; i++)
    {
        if (_pQfile->atEnd())
            break;
        QByteArray line = _pQfile->readLine();
        if (line.contains("\r\n"))
        {
            line.chop(2);
        }
        else if (line.contains("\n") || line.contains("\r"))
        {
            line.chop(1);
        }
        QList<QByteArray> rowStrs = line.split(_validDelimiterChar);
        if (i == 0)
        {
            _previewData.reserve(rowStrs.size());
            _metaData.resize(rowStrs.size());
            for (int i = 0; i < rowStrs.size(); i++)
            {
                _previewData.push_back(std::shared_ptr<std::vector<std::string>>(new std::vector<std::string>));
                _metaData[i].colName = rowStrs[i].toStdString();
            }
            continue;
        }
        for (int i = 0; i < rowStrs.size(); i++)
        {
            _previewData[i]->push_back(rowStrs[i].toStdString());
        }
    }
    _pQfile->seek(pos);
    if (_previewData.size() <= 0)
        return false;
    return parseDataType();
}

bool CsvExtractor::parseData(outdata_t &vvs, bool &bLastPack)
{
    qint64 pos = _pQfile->pos();
    vvs.clear();
    int i = 0;
    bLastPack = false;
    while (!_pQfile->atEnd() && i <= PERREADCOUNT)
    {
        QByteArray line = _pQfile->readLine();
        if (i == 0)
        {
            i++;
            continue;
        }
        if (line.contains("\r\n"))
        {
            line.chop(2);
        }
        else if (line.contains("\n") || line.contains("\r"))
        {
            line.chop(1);
        }
        QList<QByteArray> rowStrs = line.split(_validDelimiterChar);
        if (i == 1)
        {
            vvs.reserve(rowStrs.size());
            for (int i = 0; i < rowStrs.size(); i++)
            {
                vvs.push_back(std::shared_ptr<std::vector<std::string>>(new std::vector<std::string>));
            }
        }
        for (int i = 0; i < rowStrs.size(); i++)
        {
            vvs[i]->push_back(rowStrs[i].toStdString());
        }
        i++;
    }
    if (i < PERREADCOUNT)
    {
        bLastPack = true;
    }
    return true;
}


bool CsvExtractor::parseDataType()
{

    // parse 100 row recorder
    outdata_t::iterator spVsItor = _previewData.begin();
    int colIdx = 0;
    for (; spVsItor != _previewData.end(); spVsItor++, colIdx++)
    {
        bool bAllNull = true;
        bool bDouble = true;
        bool bDate = true;
        bool bInt = true;
        bool bYearCol = true;
        DataType col_dt;
        std::string date_Format = "M/D/YYYY";
        char guessSep = -1,thSep = -1,cDecimal = -1;

        int checkLines = _previewData[0]->size();

        for (int i = 1; i < checkLines; i++)
        {
            if ((**spVsItor)[i].empty())
                continue;

            bAllNull = false;

            // Year column length must be 4 -> "2017" or 2 -> "17"
            if (bYearCol && ((**spVsItor)[i].length() != 4 && (**spVsItor)[i].length() != 2) )
                bYearCol = false;

            col_dt = getFieldType((**spVsItor)[i], date_Format, guessSep, thSep, cDecimal);

            if (col_dt == DT_TEXT)
            {
                bDouble = false;
                bInt = false;
                bDate = false;
                date_Format = "";
                break;	// if meet string field stop check at once, this column must be string column
            }

            // NOTE: cannot change three if order. Double could include Int, Int could include Date(like:20160412)
            if (bDouble && col_dt == DT_DOUBLE)
            {
                bInt = false;
                bDate = false;
                date_Format = "";
                continue;	// if current field is double, it couldn't is Int and Date anymore
            }
            if (bInt && col_dt == DT_INTEGER)
            {
                bDate = false;
                date_Format = "";
                continue;
            }

            // else must be DATE
        }

        // all of 100 row are null , think it as TEXT
        if (bDate && !bAllNull)
        {
            _metaData[colIdx].colType = DT_DATE;
            _metaData[colIdx].colFormat = date_Format;
        }
        else if (bInt && !bAllNull)
        {
            if (bYearCol)
            {
                std::string colName;
                QString lowerColName = QString((_metaData[colIdx].colName).c_str()).toLower();
                if (lowerColName.contains("year", Qt::CaseInsensitive))
                    _metaData[colIdx].colType = DT_TEXT;
                else
                    _metaData[colIdx].colType = DT_INTEGER;
            }
            else
                _metaData[colIdx].colType = DT_INTEGER;
        }
        else if (bDouble && !bAllNull)
            _metaData[colIdx].colType = DT_DOUBLE;
        else
            _metaData[colIdx].colType = DT_TEXT;

        if (!bDate || bAllNull)
             _metaData[colIdx].colFormat == "";
    }

    return true;
}

bool CsvExtractor::isDateTime(const std::string &str, std::string& format, bool hasFormat)
{
    if (str.size() < 6)
        return false;
    CVDate dt;
    format = dt.GetFormat(str.c_str(), format.c_str(), true);
    if (format.length())
        return true;
    return false;
}

void CsvExtractor::initCurrencySymbol()
{
    // Support Currency type, can insert other type before NULL to this array for extending.
    // UTF-8 encode
    char currency[][6] =
    {
        {(char)0x24, (char)0x00},							// $
        {(char)0xe2, (char)0x82, (char)0xac, (char)0x00},	// Euro Member Countries
        {(char)0xc2, (char)0xa3, (char)0x00},				// Egypt Pound
        {(char)0xc2, (char)0xa5, (char)0x00},				// China Yuan Renminbi £¤
        {(char)0xe2, (char)0x82, (char)0xa9, (char)0x00},	// Korea (North) Won  (w=)
        "CHF",												// Switzerland Franc
        "Kr",												// Denmark Krone
        {0}
    };
    int i = 0;
    while (currency[i] && *currency[i])
        s_vecCurrecny.push_back(currency[i++]);
}

bool CsvExtractor::hasCurrencySymbol(std::string& str)
{
    QString qstr(str.c_str());
    for (unsigned int i = 0; i < s_vecCurrecny.size(); i++)
    {
        if (qstr.contains(QString(s_vecCurrecny[i].c_str())))
        {
            return true;
        }
    }
    return false;
}

void CsvExtractor::disposeSpecialSE(std::string& str, bool &bDate)
{
    bool bNegative = false;
    QString qstr(str.c_str());
    QString strTrim = qstr.trimmed();
    int startPos = 0;
    int endPos = strTrim.size();


    // special negative express
    if (strTrim[startPos] == '(' && strTrim[endPos - 1] == ')')
    {
        bDate = false;
        startPos++;
        endPos--;
        bNegative = true;
    }

    // percentage
    if (endPos > 0 && strTrim[endPos - 1] == '%')
    {
        endPos--;
        bDate = false;
    }

    // If has ( ) hasn't other sign
    if (bNegative)
    {
        QString tmp = strTrim.mid(startPos, endPos - 1 - startPos);
        str = tmp.toStdString();
        return;
    }

    if (startPos < endPos && strTrim[startPos] == '-')
    {
        bDate = false;
        startPos++;
    }
    else if (endPos > 0 && strTrim[endPos - 1] == '-') // special negative express , negative symbol at end position
    {
        endPos--;
        bDate = false;
    }
    else if (startPos< endPos && strTrim[startPos] == '+')
    {
        bDate = false;
        startPos++;
    }
    QString tmp = strTrim.mid(startPos, endPos - startPos);
    str = tmp.toStdString();
}
bool CsvExtractor::isThousandSep(const char c)
{
    for (unsigned int i = 0; i < sizeof(cThousandSep); i++)
    {
        if (c == cThousandSep[i])
            return true;
    }
    return false;
}
bool CsvExtractor::isDecimalSep(const char c)
{
    for (unsigned int i = 0; i < sizeof(cDecimalSep);i++)
    {
        if (c == cDecimalSep[i])
            return true;
    }
    return false;
}

DataType CsvExtractor::getFieldType(std::string& str, std::string &strDateFmt, char &guessSep, char &thSep, char &cDeSep)
{
    bool bCurrency;
    bool bDate = strDateFmt.length() ? true : false;
    bool bDecimal = false;
    int sepCount = 0;		// thousand separator count
    int sepPos = -1;				// current separator position
    char currentSep = -1;	// current separator
    bool bMeet_E = false;
    int i = 0;
    if (str == "")
        return DT_TEXT;

    disposeSpecialSE(str, bDate);
    bCurrency = hasCurrencySymbol(str);

    if (bCurrency)	// if has currency symbol need check again, after it maybe has space, sign, Like : $-8404325, $ 8404325
        disposeSpecialSE(str, bDate);
    // if pchbuf lengthen short than 5  or longer than 64 this is not a DATE, the shortest date format is D/M/YY or M/D/YY
    if (!bCurrency && bDate && str.size() > 5 && str.size() < 64)
    {
        if (isDateTime(str, strDateFmt, false))	// date maybe include string, number so check it first
            return DT_DATE;
    }

    // parse left string from i~end
    for (; i < str.size(); i++)
    {
        if ((QChar(str[i]).isNumber()))
        {
            if (i == 0 && str[i] == '0' && (i + 1 != str.size() && !isDecimalSep(str[i + 1])) )
                return DT_TEXT;	// the first number cannot is '0' or it is '0' next it must be decimal '0.123'
            continue;
        }

        if (isThousandSep(str[i]))
        {
            if (bDecimal || i + 1 == str.size())	// separator cannot be last position '123,' think as text
                return DT_TEXT;	// once ever met decimal, cannot meet any separators!

            sepCount++;	// count thousand separator

            if (currentSep == -1)	// first time meet separator
            {
                currentSep = str[i];
                sepPos = i;
            }
            else
            {
                if (currentSep == str[i])	// thousand separator must same always
                {
                    if ((i - sepPos - 1) != 3) // thousand separator must 3 position
                        return DT_TEXT;
                    sepPos = i;
                    if (thSep == -1)
                    {
                        thSep = str[i];	// meet more than twice and same, this must be thousand separator
                        if (guessSep != -1 && guessSep != thSep)
                        {
                            if (isDecimalSep(guessSep))
                            {
                                bDecimal = true;
                                cDeSep = guessSep;
                                guessSep = -1;
                            }
                            else
                                return DT_TEXT;
                        }
                    }
                    else if(thSep != str[i])	// one column all thousand separator must be same
                        return DT_TEXT;
                    guessSep = -1; //guessSep invalid
                }
                else if (isDecimalSep(str[i]))	// thousand separator array include decimal separators
                {
                    bDecimal = true;
                    thSep = currentSep;
                    if (cDeSep == -1)
                        cDeSep = str[i];	// set decimal separator
                    else if (cDeSep != str[i])
                        return DT_TEXT;
                    guessSep = -1; //guessSep invalid
                }
                else
                    return DT_TEXT;
            }
        }

        // Special character: e or E -> before 'e/E' must be digit && follow 'e/E' must be digit or +digit or -digit
        else if ((str[i] == 'e' || str[i] == 'E') &&
                 ((i > 0) && QChar(str[i - 1]).isNumber()) &&
                 (
                     (
                         (str.size() - i) == 1 && QChar(str[i + i]).isNumber()
                      ) ||
                     (
                         (str.size() - i) > 1  &&
                         (
                             QChar(str[i + 1]).isNumber() ||
                             (
                                 (str[i + 1] == '-' || str[ i + 1] == '+') &&
                                 QChar(str[i + 2]).isNumber()
                              )
                          )
                      )
                  )
                 )
        {
            bMeet_E = true;
            break;
        }
        else	// all of other status think as TEXT
            return DT_TEXT;
    }

    // only have one separator, it maybe thousand , maybe decimal, maybe all not.
    if (sepCount == 1 )
    {
        if ((i - sepPos - 1) != 3)	// separator only appear once, between it to 'e'/'E' != 3, it maybe decimal
        {
            if (isDecimalSep(str[sepPos]))
            {
                bDecimal = true;
                if (cDeSep == -1)
                    cDeSep = str[sepPos];	// set decimal separator
                else if (cDeSep != str[sepPos])
                    return DT_TEXT;
                guessSep = -1; //guessSep invalid
            }
            else
                return DT_TEXT;
        }
        else
        {
            if (sepPos<= 3) // separator only appear once, after it must is 3 digit, before it muse less than 3 digit
            {
                 if (thSep == -1 && cDeSep == -1)
                 {
                     if (guessSep == -1)
                        guessSep = str[sepPos];
                     else if (guessSep != str[sepPos])	// has two different separator
                     {
                         bDecimal = true;
                         guessSep =-1;
                     }

                    //else guessSep alway same 12,123   12.123
                    //						    1,345    1.345
                    //						  123,456  123.456

                 }
            }
            else
            {
                if (isDecimalSep(str[sepPos]))
                {
                    bDecimal = true;
                    if (cDeSep == -1)
                        cDeSep = str[sepPos];	// set decimal separator
                    else if (cDeSep != str[sepPos])
                        return DT_TEXT;
                    guessSep = -1; //guessSep invalid
                }
                else
                    return DT_TEXT;
            }
        }
    }

    if (bMeet_E)
    {
        i++;	// skip 'e'
        i++;	// skip after e first character maybe 'digit' or '-' or '+'
        while(i < str.size())	// left chars must be all digit
        {
            if (!QChar(str[i++]).isNumber())
                return DT_TEXT;
        }

        return DT_DOUBLE;
    }
    if (bDecimal)
        return DT_DOUBLE;

    return DT_INTEGER;
}
