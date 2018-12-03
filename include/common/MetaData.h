#ifndef MetaData_INCLUDED
#define MetaData_INCLUDED


#include <string>
#include <vector>
#include <memory>


enum SourceType {
    ST_CSV = 0
};

struct SourceInfo
{
    virtual ~SourceInfo(){};
    typedef std::shared_ptr<SourceInfo> Ptr;
    SourceType sourceType;
    std::string sourceTypeName;
    std::string sourceID;
    std::string sourceName;
    virtual std::string makeDetail() = 0;
    virtual bool unmakeDetail(std::string& sourceDetail) = 0;
    static std::string sourceTypeToString(SourceType sourceType);
    static std::shared_ptr<SourceInfo> fromDBWithSourceID(std::string& sourceID);
    static std::vector<std::shared_ptr<SourceInfo>> fromDBWithSourceName(std::string& sourceName);
    static std::vector<std::shared_ptr<SourceInfo>> fromDBWithSourceType(SourceType& sourceType);
    static std::vector<std::shared_ptr<SourceInfo>> fromDB();
    static bool removeFromDB(std::string& sourceID);
    static bool toDB(std::shared_ptr<SourceInfo> sourceInfo);
};

struct CSVSourceInfo : public SourceInfo
{
    typedef std::shared_ptr<CSVSourceInfo> Ptr;
    virtual ~CSVSourceInfo(){}
    std::string sourcePath;
    virtual std::string makeDetail();
    virtual bool unmakeDetail(std::string& sourceDetail);
};

struct QueryInfo
{
    SourceInfo::Ptr pSourceInfo;
    std::string queryID;
    std::string queryName;
    int nodeNum;
    static std::shared_ptr<QueryInfo> fromDBWithQueryID(std::string& queryID);
    static std::vector<std::shared_ptr<QueryInfo>> fromDBWithQueryName(std::string& queryName);
    static std::vector<std::shared_ptr<QueryInfo>> fromDBWithSourceID(std::string& sourceID);
    static bool toDB(std::shared_ptr<QueryInfo> queryNode);
};

enum DataType {
  DT_INTEGER = 0,
  DT_DOUBLE = 1,
  DT_TEXT = 2,
  DT_DATE = 3,
  DT_CODE = 4,
  DT_CURRENCY = 5,
  DT_LONGITUDE = 6,
  DT_LATITUDE = 7,
  DT_TIME = 8,
  DT_NUM = 9
};

struct TableColumnInfo
{
    typedef std::shared_ptr<TableColumnInfo> Ptr;
    std::string nodeID;
    int colIndex;
    std::string colID;
    std::string colName;
    std::string colFormat;
    DataType colType;
    bool used;
    static std::vector<TableColumnInfo::Ptr> fromDBWithNodeID(std::string& nodeID);
    static bool toDBWithNodeID(std::string& nodeID, std::vector<TableColumnInfo::Ptr>);
};

struct QueryNode
{
    std::string operationOnNode;
    std::string queryID;
    std::string nodeID;
    int nodeIndex;
    std::vector<TableColumnInfo::Ptr> pTableColumnInfo;
    static std::shared_ptr<QueryNode> fromDBWithNodeID(std::string& nodeID);
    static std::vector<std::shared_ptr<QueryNode>> fromDBWithQueryID(std::string& queryID);
    static bool toDB(std::shared_ptr<QueryNode> queryNode);
};

#endif
