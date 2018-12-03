#ifndef DataTableModel_H
#define DataTableModel_H

#include <QtCore/QAbstractTableModel>
#include <QtCore/QHash>
#include <QtCore/QRect>
#include "include/common/MetaData.h"
#include "include/Extractor.h"
#include "include/view/MetaDataQML.h"
class DataTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:

    explicit DataTableModel(QObject *parent = 0);
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    Q_INVOKABLE bool appendRows(const outdata_t& outData);
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void setTableCoulumnInfos(std::vector<std::shared_ptr<TableColumnInfoQML>>& tableColumnsInfos);
    void setWithOutData(outdata_t &vvs);
    void appendDataWithOutData(outdata_t &vvs);
protected:

private:
    QList<QVector<QString>> m_data;
    int m_rowCount;
    std::vector<std::shared_ptr<TableColumnInfoQML>> m_tableColumnsInfos;
    bool m_isAlldata;
};

#endif // DATATABLEMODEL_H
