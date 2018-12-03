#ifndef DATAVIEW_H
#define DATAVIEW_H
#include <QObject>
#include <QUrl>
#include <QDebug>
#include <memory>
#include <QQmlListProperty>
#include "include/view/DataTableModel.h"
#include "include/view/MetaDataQML.h"
class QQuickItem;
class CsvExtractor;
class DataViewQML : public QObject
{
Q_OBJECT
public:
    explicit DataViewQML(QObject *parent = nullptr);
    Q_INVOKABLE QList<QString> loadHistoryDatabase();
    Q_INVOKABLE bool uploadFile(QUrl filePath);
    Q_INVOKABLE QList<QString> getTableList(int databaseIndex);
    Q_INVOKABLE QString getDataBaseName(int databaseIndex);
    Q_INVOKABLE bool deleteDataBase(int databaseIndex);

    Q_INVOKABLE bool selectTable(int databaseIndex, QString tabName);
    Q_INVOKABLE QVariant getMetaData();
    Q_INVOKABLE QVariant getPreViewData();
    Q_INVOKABLE bool createQuery();
//    Q_INVOKABLE QVariant getAllData(bool firstPack);
    Q_INVOKABLE bool finishingGetData();
private:

    bool validFilePath(QUrl& filePath);
    std::vector<std::shared_ptr<Extractor>> extractorVec;
    std::vector<std::shared_ptr<TableColumnInfoQML>> _pTableColumnInfoQMLVec;
    std::vector<std::shared_ptr<SourceInfo>> _sourceInfoVec;
    QSharedPointer<DataTableModel> pDataTableModeForAllData;
    bool _isAllData;
    int _currentDataBaseIndex;
    QString _currentTableName;
};

#endif // DATAVIEW_H
