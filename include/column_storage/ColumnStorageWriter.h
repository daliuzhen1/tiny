#ifndef ColumnStorageWriter_INCLUDED
#define ColumnStorageWriter_INCLUDED

#include <arrow/api.h>
#include <arrow/io/api.h>
#include <parquet/arrow/reader.h>
#include <parquet/arrow/writer.h>
#include <parquet/exception.h>

#include <QDebug>

#include "include/common/MetaData.h"
#include "include/Extractor.h"

class ColumnStorageWriter
{
public:
    typedef std::shared_ptr<ColumnStorageWriter> Ptr;
    ColumnStorageWriter(std::vector<TableColumnInfo>& colInfos, std::string& outFileName);
    ColumnStorageWriter(ColumnStorageWriter&) = delete;
    void write_batch(outdata_t& data, bool lastPack);
private:
    void setupSchema();
    std::vector<TableColumnInfo> &_colInfos;
    std::shared_ptr<parquet::schema::GroupNode> _schema;
    std::string _outFileName;
    std::shared_ptr<arrow::io::FileOutputStream> _outPutStream;
    std::shared_ptr<parquet::ParquetFileWriter> _parquetFileWriter;
    parquet::RowGroupWriter* _curr_rg_writer;
};


#endif
