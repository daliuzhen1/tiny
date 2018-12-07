#include "include/column_storage/ColumnStorageWriter.h"
#include <parquet/file_writer.h>
#include <arrow/table_builder.h>
#include <QString>
ColumnStorageWriter::ColumnStorageWriter(std::vector<std::shared_ptr<TableColumnInfo>>& colInfos, std::string& outFileName):
    _colInfos(colInfos),
    _outFileName(outFileName),
    _curr_rg_writer(nullptr)
{
    qDebug() << "ColumnStorageWriter::ColumnStorageWriter";
    PARQUET_THROW_NOT_OK(arrow::io::FileOutputStream::Open(_outFileName.c_str(), &_outPutStream));
    setupSchema();
    parquet::WriterProperties::Builder builder;
    builder.compression(parquet::Compression::SNAPPY);
    std::shared_ptr<parquet::WriterProperties> props = builder.build();
    _parquetFileWriter = parquet::ParquetFileWriter::Open(_outPutStream, _schema, props);
}


void ColumnStorageWriter::setupSchema()
{
    parquet::schema::NodeVector fields;
    for (unsigned int i = 0; i < _colInfos.size(); i++)
    {
        parquet::schema::NodePtr field;
        switch (_colInfos[i]->colType)
        {
        case DT_INTEGER:
            field = parquet::schema::PrimitiveNode::Make(_colInfos[i]->colName, parquet::Repetition::REQUIRED,
                                                          parquet::Type::INT64, parquet::LogicalType::NONE);
            break;
        case DT_DOUBLE:
            field = parquet::schema::PrimitiveNode::Make(_colInfos[i]->colName, parquet::Repetition::REQUIRED,
                                                          parquet::Type::DOUBLE, parquet::LogicalType::NONE);
            break;
        case DT_TEXT:
            field = parquet::schema::PrimitiveNode::Make(_colInfos[i]->colName, parquet::Repetition::REQUIRED,
                                                          parquet::Type::BYTE_ARRAY, parquet::LogicalType::NONE);
            break;
        case DT_DATE:
            field = parquet::schema::PrimitiveNode::Make(_colInfos[i]->colName, parquet::Repetition::REQUIRED,
                                                          parquet::Type::INT64, parquet::LogicalType::NONE);
            break;
        case DT_CURRENCY:
            field = parquet::schema::PrimitiveNode::Make(_colInfos[i]->colName, parquet::Repetition::REQUIRED,
                                                          parquet::Type::DOUBLE, parquet::LogicalType::NONE);
            break;
        case DT_LONGITUDE:
            field = parquet::schema::PrimitiveNode::Make(_colInfos[i]->colName, parquet::Repetition::REQUIRED,
                                                          parquet::Type::FLOAT, parquet::LogicalType::NONE);
            break;
        case DT_LATITUDE:
            field = parquet::schema::PrimitiveNode::Make(_colInfos[i]->colName, parquet::Repetition::REQUIRED,
                                                          parquet::Type::FLOAT, parquet::LogicalType::NONE);
            break;
        case DT_NUM:
            field = parquet::schema::PrimitiveNode::Make(_colInfos[i]->colName, parquet::Repetition::REQUIRED,
                                                          parquet::Type::INT64, parquet::LogicalType::NONE);
            break;
        default:
            throw std::runtime_error("error format");
        }
        fields.push_back(field);
    }
    if (fields.size() <= 0)
        throw std::runtime_error("columns size error");

     _schema =  std::static_pointer_cast<parquet::schema::GroupNode>(
        parquet::schema::GroupNode::Make("schema", parquet::Repetition::REQUIRED, fields));
}

void ColumnStorageWriter::write_batch(outdata_t& data, bool lastPack)
{
    qDebug() << "ColumnStorageWriter::write_batch";
    _curr_rg_writer = _parquetFileWriter->AppendBufferedRowGroup();
    if (!_curr_rg_writer || data.size() == 0 || data[0]->size() == 0)
        throw std::runtime_error("write batch error");
    int num_columns = _parquetFileWriter->num_columns();
    for (unsigned rowIdx = 0; rowIdx < data[0]->size(); rowIdx++)
    {
        for (int colIdx = 0; colIdx < data.size(); colIdx++)
        {
            switch (_colInfos[colIdx]->colType)
            {
            case DT_INTEGER:
            {
                parquet::Int64Writer* int64_writer =
                    static_cast<parquet::Int64Writer*>(_curr_rg_writer->column(colIdx));
                int64_t value =  QString(data[colIdx]->at(rowIdx).c_str()).toInt();
                int64_writer->WriteBatch(1, nullptr, nullptr, &value);
                break;
            }
            case DT_DOUBLE:
            {
                parquet::DoubleWriter* double_writer =
                    static_cast<parquet::DoubleWriter*>(_curr_rg_writer->column(colIdx));
                double value =  QString(data[colIdx]->at(rowIdx).c_str()).toDouble();
                double_writer->WriteBatch(1, nullptr, nullptr, &value);
                break;
            }
            case DT_TEXT:
            {
                parquet::ByteArrayWriter* byte_writer =
                    static_cast<parquet::ByteArrayWriter*>(_curr_rg_writer->column(colIdx));
                std::string &value =  data[colIdx]->at(rowIdx);
                parquet::ByteArray ba_value;
                ba_value.ptr = reinterpret_cast<const uint8_t*>(value.c_str());
                ba_value.len = value.size();
                byte_writer->WriteBatch(1, nullptr, nullptr, &ba_value);
                break;
            }
            case DT_DATE:
                break;
            case DT_CURRENCY:
            {
                parquet::DoubleWriter* double_writer =
                    static_cast<parquet::DoubleWriter*>(_curr_rg_writer->column(colIdx));
                double value =  QString(data[colIdx]->at(rowIdx).c_str()).toDouble();
                double_writer->WriteBatch(1, nullptr, nullptr, &value);
                break;
            }
            case DT_LONGITUDE:
            {
                parquet::FloatWriter* float_writer =
                    static_cast<parquet::FloatWriter*>(_curr_rg_writer->column(colIdx));
                float value =  QString(data[colIdx]->at(rowIdx).c_str()).toFloat();
                float_writer->WriteBatch(1, nullptr, nullptr, &value);
                break;
            }
            case DT_LATITUDE:
            {
                parquet::FloatWriter* float_writer =
                    static_cast<parquet::FloatWriter*>(_curr_rg_writer->column(colIdx));
                float value =  QString(data[colIdx]->at(rowIdx).c_str()).toFloat();
                float_writer->WriteBatch(1, nullptr, nullptr, &value);
                break;
            }
            case DT_NUM:
            {
                parquet::Int64Writer* int64_writer =
                    static_cast<parquet::Int64Writer*>(_curr_rg_writer->column(colIdx));
                int64_t value =  QString(data[colIdx]->at(rowIdx).c_str()).toInt();
                int64_writer->WriteBatch(1, nullptr, nullptr, &value);
                break;
            }
            default:
                throw std::runtime_error("error format");
            }
        }
    }
    _curr_rg_writer->Close();
    if (lastPack)
    {
        _parquetFileWriter->Close();
    }
    qDebug() << "ColumnStorageWriter::finish";
}
