#ifndef Extractor_H
#define Extractor_H

#include <memory>
#include "include/common/MetaData.h"
typedef std::vector<std::shared_ptr<std::vector<std::string> > > outdata_t;
class Extractor
{
public:
    virtual ~Extractor(){}
    std::vector<TableColumnInfo>& getMetaData();
    outdata_t& getPreviewData();
    virtual bool parseData(outdata_t &vvs, bool &bLastPack) = 0;
    virtual std::shared_ptr<std::vector<std::string>> getTableList() = 0;
    virtual bool selectTable(std::string& name) = 0;
    virtual std::string& getDatabaseName() = 0;

protected:
    std::vector<TableColumnInfo> _metaData;
    outdata_t _previewData;
};

#endif // Extractor_H
