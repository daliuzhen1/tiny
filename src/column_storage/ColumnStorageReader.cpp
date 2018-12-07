#include "include/column_storage/ColumnStorageReader.h"

#define NUMBEROFREAD 10
ColumnStorageReader::ColumnStorageReader(std::vector<std::shared_ptr<TableColumnInfo>>& colInfos, std::string& inFileName) : _colInfos(colInfos),
    _inFileName(inFileName),
    _current_rg_idx(0)
{
    _parquetFileReader = parquet::ParquetFileReader::OpenFile(inFileName);
    std::shared_ptr<parquet::FileMetaData> file_metadata = _parquetFileReader->metadata();
    int num_columns = file_metadata->num_columns();
    if (!num_columns)
        throw std::runtime_error("column num is 0");
    const parquet::SchemaDescriptor* psd = file_metadata->schema();
    for (int i = 0; i < num_columns; i++)
    {
        // TODO now only is a column name
        const parquet::ColumnDescriptor* pcd = psd->Column(i);
        std::shared_ptr<TableColumnInfo> colInfo(new TableColumnInfo) ;
        colInfo->colName = pcd->name();
        _colInfos.push_back(colInfo);
    }
}


void ColumnStorageReader::read_batch(outdata_t& data, bool& lastPack)
{
    lastPack = false;
    data.clear();
    int columnNum = _parquetFileReader->metadata()->num_columns();
    data.reserve(columnNum);
    for (int i = 0; i < columnNum; i++)
    {
        data.push_back(std::shared_ptr<std::vector<std::string>>(new std::vector<std::string>));
    }
    if (_current_rg_idx < _parquetFileReader->metadata()->num_row_groups())
    {
        std::shared_ptr<parquet::RowGroupReader> row_group_reader =
                _parquetFileReader->RowGroup(_current_rg_idx);
        for (int i = 0; i < columnNum; i++)
        {
            std::shared_ptr<parquet::ColumnReader> column_reader = row_group_reader->Column(i);
            switch (_colInfos.at(i)->colType)
            {
            case DT_INTEGER:
            {
                parquet::Int64Reader* int64_reader =
                        static_cast<parquet::Int64Reader*>(column_reader.get());
                while (int64_reader->HasNext())
                {
                    int64_t values[NUMBEROFREAD] = {0};
                    int64_t numberOfNotNull = 0;
                    int64_t read_size = int64_reader->ReadBatch(NUMBEROFREAD, nullptr, nullptr, values, &numberOfNotNull);
                    if (numberOfNotNull != read_size)
                        throw std::runtime_error("read DT INTEGER error");
                    for (int readIdx = 0; readIdx < read_size; readIdx++)
                    {
                        data[i]->push_back(QString::number(values[readIdx]).toStdString());
                    }
                }
                break;
            }
            case DT_TEXT:
            {
                parquet::ByteArrayReader* ba_reader =
                        static_cast<parquet::ByteArrayReader*>(column_reader.get());
                while (ba_reader->HasNext())
                {
                    parquet::ByteArray values[10] = {};
                    int64_t numberOfNotNull = 0;
                    int64_t read_size = ba_reader->ReadBatch(NUMBEROFREAD, nullptr, nullptr, values, &numberOfNotNull);
                    if (numberOfNotNull != read_size)
                        throw std::runtime_error("read DT TEXT error");
                    for (int readIdx = 0; readIdx < read_size; readIdx++)
                    {
                        data[i]->push_back(reinterpret_cast<const char*>(values[readIdx].ptr));
                    }
                }
                break;
            }
            case DT_DATE:
            {
                break;
            }
            case DT_CODE:
            {
                break;
            }
            case DT_CURRENCY:
            case DT_LONGITUDE:
            case DT_LATITUDE:
            case DT_DOUBLE:
            {
                parquet::DoubleReader* double_reader =
                        static_cast<parquet::DoubleReader*>(column_reader.get());
                while (double_reader->HasNext())
                {
                    double values[NUMBEROFREAD] = {0};
                    int64_t numberOfNotNull = 0;
                    int64_t read_size = double_reader->ReadBatch(NUMBEROFREAD, nullptr, nullptr, values, &numberOfNotNull);
                    if (numberOfNotNull != read_size)
                        throw std::runtime_error("read DT INTEGER error");
                    for (int readIdx = 0; readIdx < read_size; readIdx++)
                    {
                        data[i]->push_back(QString::number(values[readIdx]).toStdString());
                    }
                }
                break;
            }
            case DT_TIME:
            {
                break;
            }
            case DT_NUM:
            {
                parquet::Int64Reader* int64_reader =
                        static_cast<parquet::Int64Reader*>(column_reader.get());
                while (int64_reader->HasNext())
                {
                    int64_t values[NUMBEROFREAD] = {0};
                    int64_t numberOfNotNull = 0;
                    int64_t read_size = int64_reader->ReadBatch(NUMBEROFREAD, nullptr, nullptr, values, &numberOfNotNull);
                    if (numberOfNotNull != read_size)
                        throw std::runtime_error("read DT INTEGER error");
                    for (int readIdx = 0; readIdx < read_size; readIdx++)
                    {
                        data[i]->push_back(QString::number(values[readIdx]).toStdString());
                    }
                }
                break;
            }
            default:
                break;
            }

        }
        _current_rg_idx++;
    }

    if (_current_rg_idx == _parquetFileReader->metadata()->num_row_groups())
        lastPack = true;
    return;
}


std::vector<std::shared_ptr<TableColumnInfo>>& ColumnStorageReader::getColumnInfo()
{
    return _colInfos;
}
