#include "include/view/MetaDataQML.h"
TableColumnInfoQML::TableColumnInfoQML(QObject *parent): QObject(parent)
{

}


std::shared_ptr<TableColumnInfo> TableColumnInfoQML::covertFromQML(TableColumnInfoQML& tableColumnInfoQML)
{
    std::shared_ptr<TableColumnInfo> pTableColumnInfo = std::shared_ptr<TableColumnInfo>(new TableColumnInfo);
    pTableColumnInfo->colID = tableColumnInfoQML.colID().toStdString();
    pTableColumnInfo->colName = tableColumnInfoQML.colName().toStdString();
    pTableColumnInfo->colFormat = tableColumnInfoQML.colFormat().toStdString();
    pTableColumnInfo->colType = (DataType)tableColumnInfoQML.colType();
    pTableColumnInfo->colIndex = tableColumnInfoQML.colIndex();
    pTableColumnInfo->used = tableColumnInfoQML.used();
    return pTableColumnInfo;
}

std::shared_ptr<TableColumnInfoQML> TableColumnInfoQML::covertFromCPP(TableColumnInfo& tableColumnInfo)
{
    std::shared_ptr<TableColumnInfoQML> pTableColumnInfoQML = std::shared_ptr<TableColumnInfoQML>(new TableColumnInfoQML);
    pTableColumnInfoQML->setColID(tableColumnInfo.colID.c_str());
    pTableColumnInfoQML->setColIndex(tableColumnInfo.colIndex);
    pTableColumnInfoQML->setColName(tableColumnInfo.colName.c_str());
    pTableColumnInfoQML->setColFormat(tableColumnInfo.colFormat.c_str());
    TableColumnInfoQML::DataTypeQML dtq = (TableColumnInfoQML::DataTypeQML)(tableColumnInfo.colType);
    pTableColumnInfoQML->setColType(dtq);
    pTableColumnInfoQML->setUsed(tableColumnInfo.used);
    return pTableColumnInfoQML;
}
