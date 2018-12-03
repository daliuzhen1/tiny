#ifndef ColumnStorageReader_INCLUDED
#define ColumnStorageReader_INCLUDED

#include <arrow/api.h>
#include <arrow/io/api.h>
#include <parquet/arrow/reader.h>
#include <parquet/arrow/writer.h>
#include <parquet/exception.h>

#include <QDebug>

#include "include/common/MetaData.h"
#include "include/Extractor.h"

class ColumnStorageReader
{
public:
    typedef std::shared_ptr<ColumnStorageReader> Ptr;
    ColumnStorageReader(std::vector<TableColumnInfo>& colInfos, std::string& inFileName);
    ColumnStorageReader(ColumnStorageReader&) = delete;
    void read_batch(outdata_t& data, bool& lastPack);
    std::vector<TableColumnInfo>& getColumnInfo();
private:
    std::vector<TableColumnInfo> _colInfos;
    std::string _inFileName;
    std::shared_ptr<arrow::io::FileOutputStream> _outPutStream;
    std::shared_ptr<parquet::ParquetFileReader> _parquetFileReader;
    int _current_rg_idx;
};




#endif
