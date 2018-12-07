#ifndef MetaDataQML_INCLUDED
#define MetaDataQML_INCLUDED


#include <string>
#include <vector>
#include <QObject>
#include <QUrl>
#include <QDebug>
#include <memory>

#include "include/common/MetaData.h"
class TableColumnInfoQML : public QObject
{
    Q_OBJECT
    Q_ENUMS(DataTypeQML)
public:
    explicit TableColumnInfoQML(QObject *parent = nullptr);
    enum DataTypeQML {
      DT_INTEGER = 0,
      DT_DOUBLE = 1,
      DT_TEXT = 2,
      DT_DATE = 3,
      DT_CODE = 4,
      DT_CURRENCY = 5,
      DT_LONGITUDE = 6,
      DT_LATITUDE = 7,
      DT_TIME = 8,
      DT_NUM = 9
    };

    Q_PROPERTY(QString colID READ colID WRITE setColID)
    Q_PROPERTY(QString colName READ colName WRITE setColName)
    Q_PROPERTY(QString colFormat READ colFormat WRITE setColFormat)
    Q_PROPERTY(DataTypeQML colType READ colType WRITE setColType)
    Q_PROPERTY(bool used READ used WRITE setUsed)
    Q_PROPERTY(int colIndex READ colIndex WRITE setColIndex)
public:

    void setColID(QString colID)
    {
        _colID = colID;
    }
    QString colID()
    {
        return _colID;
    }

    void setColName(QString colName)
    {
        _colName = colName;
    }

    QString colName()
    {
        return _colName;
    }

    void setColFormat(QString colFormat)
    {
        _colFormat = colFormat;
    }

    QString colFormat()
    {
        return _colFormat;
    }

    void setColType(DataTypeQML colType)
    {
        _colType = colType;
    }

    DataTypeQML colType()
    {
        return _colType;
    }

    void setUsed(bool used)
    {
        _used = used;
    }

    bool used()
    {
        return _used;
    }

    void setColIndex(int colIndex)
    {
        _colIndex = colIndex;
    }

    int colIndex()
    {
        return _colIndex;
    }

    static std::shared_ptr<TableColumnInfo> covertFromQML(TableColumnInfoQML& tableColumnInfoQML);
    static std::shared_ptr<TableColumnInfoQML> covertFromCPP(TableColumnInfo& tableColumnInfo);
private:
    QString _colID;
    QString _colName;
    QString _colFormat;
    DataTypeQML _colType;
    bool _used;
    int _colIndex;
};

class QueryNodeQML : public QObject
{
Q_OBJECT
public:
    explicit QueryNodeQML(QObject *parent = nullptr){};

    Q_PROPERTY(QString queryID READ queryID WRITE setQueryID)
    Q_PROPERTY(QString nodeID READ nodeID WRITE setNodeID)
    Q_PROPERTY(int nodeIndex READ nodeIndex WRITE setNodeIndex)
    Q_PROPERTY(QString nodeName READ nodeName WRITE setNodeName)
    Q_PROPERTY(QString operationOnNode READ operationOnNode WRITE setOperationOnNode)
    Q_PROPERTY(QVariant tableColumnInfoList READ tableColumnInfoList)

public:

    void setQueryID(QString queryID)
    {
        _queryID = queryID;
    }
    QString queryID()
    {
        return _queryID;
    }

    void setNodeID(QString nodeID)
    {
        _nodeID = nodeID;
    }

    QString nodeID()
    {
        return _nodeID;
    }

    void setNodeIndex(int nodeIndex)
    {
        _nodeIndex = nodeIndex;
    }

    int nodeIndex()
    {
        return _nodeIndex;
    }

    void setNodeName(QString nodeName)
    {
        _nodeName = nodeName;
    }

    QString nodeName()
    {
        return _nodeName;
    }

    void setOperationOnNode(QString operationOnNode)
    {
        _operationOnNode = operationOnNode;
    }

    QString operationOnNode()
    {
        return _operationOnNode;
    }

    QVariant tableColumnInfoList()
    {
         std::vector<std::shared_ptr<TableColumnInfoQML>> pTableColumnInfoQMLVec;
        QVariantList qList;
        for (unsigned int i = 0; i < _pTableColumnInfo.size(); i++)
        {
            std::shared_ptr<TableColumnInfoQML> pTableColumnInfoQML = TableColumnInfoQML::covertFromCPP(*(_pTableColumnInfo[i]));
            pTableColumnInfoQMLVec.push_back(pTableColumnInfoQML);
            qList.append((QVariant::fromValue<TableColumnInfoQML*>(pTableColumnInfoQML.get())));
        }
        return QVariant::fromValue(qList);
    }


    static std::shared_ptr<QueryNode> covertFromQML(QueryNodeQML& queryNodeQML);
    static std::shared_ptr<QueryNodeQML> covertFromCPP(QueryNode& queryNode);

    std::vector<TableColumnInfo::Ptr>& getTableColumnInfo();
    void setTableColumnInfo(std::vector<TableColumnInfo::Ptr>& pTableColumnInfo);
private:
    QString _operationOnNode;
    QString _queryID;
    QString _nodeID;
    QString _nodeName;
    int _nodeIndex;
    std::vector<TableColumnInfo::Ptr> _pTableColumnInfo;
};

#endif
