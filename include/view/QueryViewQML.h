#ifndef QueryViewQML_H
#define QueryViewQMLw_H
#include <QObject>
#include <QUrl>
#include <QDebug>
#include <memory>
#include <QQmlListProperty>
#include "include/view/DataTableModel.h"
#include "include/view/MetaDataQML.h"
class QQuickItem;
class CsvExtractor;

class QueryViewQML : public QObject
{
Q_OBJECT
public:
    explicit QueryViewQML(QObject *parent = nullptr);
    Q_PROPERTY(QString queryName READ queryName WRITE setQueryName NOTIFY QueryNameChanged)
    Q_PROPERTY(QString nodeName READ nodeName WRITE setNodeName NOTIFY NodeNameChanged)

    Q_INVOKABLE QVariant getMetaData();


    Q_INVOKABLE QVariant getColumnInfo(int index)
    {
        return QVariant::fromValue<TableColumnInfoQML*>(pTableColumnInfoQMLVec.at(index).get());
    }

    Q_INVOKABLE TableColumnInfoQML setColumnInfo(int index, QVariant value)
    {
        QObject * obj = qvariant_cast<QObject *>(value);
        TableColumnInfoQML *pTableColumnInfoQML = qobject_cast<TableColumnInfoQML*>(obj);
        std::shared_ptr<TableColumnInfo> pTableColumnInfo = TableColumnInfoQML::covertFromQML(*pTableColumnInfoQML);
    }

    void setQueryName(QString queryName)
    {
        _queryName = queryName;
        emit QueryNameChanged();
        return;
    }
    QString queryName()
    {
        return _queryName;
    }

    void setNodeName(QString nodeName)
    {
        _nodeName = nodeName;
        emit NodeNameChanged();
        return;
    }
    QString nodeName()
    {
        return _nodeName;
    }


private:
    QString _queryName;
    QString _nodeName;
    std::vector<std::shared_ptr<TableColumnInfoQML>> pTableColumnInfoQMLVec;
signals:
    void QueryNameChanged();
    void NodeNameChanged();
};

#endif // DATAVIEW_H
