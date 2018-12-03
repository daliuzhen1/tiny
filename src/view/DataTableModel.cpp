#include <QVector>
#include <QRandomGenerator>
#include <QRect>
#include <QColor>
#include "include/view/DataTableModel.h"
#include <QDebug>
DataTableModel::DataTableModel(QObject *parent) :
    QAbstractTableModel(parent),
    m_rowCount (0),
    m_isAlldata(false)
{
}

int DataTableModel::rowCount(const QModelIndex &parent) const
{
    return m_data.count();
}

int DataTableModel::columnCount(const QModelIndex &parent) const
{
    return m_tableColumnsInfos.size();
}

bool DataTableModel::appendRows(const outdata_t& outData)
{
    if (outData.size() <= 0 || outData[0]->size() <= 0)
        return false;

    for (int rowIdx = 0; rowIdx < outData[0]->size(); rowIdx++)
    {
        QVector<QString> vecStr;
        for (int colIndex = 0; colIndex < outData.size(); colIndex++)
        {
            vecStr.push_back(outData[colIndex]->at(rowIdx).c_str());
        }
        m_data.push_back(vecStr);
    }
    return true;
}


QVariant DataTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    qDebug() << "headerData";
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        QString colName = (const QString)m_tableColumnsInfos.at(section)->colName();
        return colName;
    } else {
        return QString("%1").arg(section + 1);
    }
}

QVariant DataTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        return m_data[index.row()].at(index.column());
    } else if (role == Qt::EditRole) {

        return m_data[index.row()].at(index.column());
    } else if (role == Qt::BackgroundRole) {
        return QColor(Qt::white);
    }
    if (role > Qt::UserRole)
    {
        return m_data[index.row()].at(role - 1 - Qt::UserRole);
    }
    return QVariant();
}

bool DataTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role > Qt::UserRole)
    {
        m_data[index.row()].replace(role - 1 - Qt::UserRole, value.toString());
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

Qt::ItemFlags DataTableModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}


void DataTableModel::setTableCoulumnInfos(std::vector<std::shared_ptr<TableColumnInfoQML>>& tableColumnsInfos)
{
    m_tableColumnsInfos = tableColumnsInfos;
    return;
}
void DataTableModel::setWithOutData(outdata_t &vvs)
{
    m_data.clear();
    if (vvs.size() == 0)
        return;
    for (unsigned int i = 0; i < vvs[0]->size(); i++) {
        QVector<QString> dataVec;
        for (unsigned int k = 0; k < vvs.size(); k++) {
            dataVec.push_back(vvs[k]->at(i).c_str());
        }
        m_data.append(dataVec);
    }
}


void DataTableModel::appendDataWithOutData(outdata_t &vvs)
{
//    modelReset();
    if (vvs.size() == 0)
        return;
    for (unsigned int i = 0; i < vvs[0]->size(); i++) {
        QVector<QString> dataVec;
        for (unsigned int k = 0; k < vvs.size(); k++) {
            dataVec.push_back(vvs[k]->at(i).c_str());
        }
        m_data.append(dataVec);
    }
}

QHash<int, QByteArray> DataTableModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    for (int i = 0  ; i  < m_tableColumnsInfos.size(); i++)
    {
        roleNames.insert(Qt::UserRole +1 + i, m_tableColumnsInfos.at(i)->colName().toStdString().c_str());
    }
    return roleNames;
}
