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
    explicit QueryViewQML(QObject *parent, std::string& queryName, std::shared_ptr<SourceInfo> pSourceInfo, std::shared_ptr<Extractor> pExtractor);
    explicit QueryViewQML(QObject *parent, QString queryID);
    Q_PROPERTY(QString queryName READ queryName WRITE setQueryName NOTIFY QueryNameChanged)

//    Q_INVOKABLE QVariant getQueryNode();
//    Q_INVOKABLE bool selectNode(int nodeIndex);

    Q_INVOKABLE QVariant getMetaData(int index);
    Q_INVOKABLE QVariant getData(int index);

    void setQueryName(QString queryName)
    {
        _pQueryInfo->queryName = queryName.toStdString();
        emit QueryNameChanged();
        return;
    }
    QString queryName()
    {
        return _pQueryInfo->queryName.c_str();
    }


private:
    std::shared_ptr<QueryInfo> _pQueryInfo;
    std::vector<std::shared_ptr<TableColumnInfoQML>> pTableColumnInfoQMLVec;
    std::vector<std::shared_ptr<Extractor>> _pExtractoVec;

signals:
    void QueryNameChanged();
    void NodeNameChanged();
};

#endif // DATAVIEW_H
