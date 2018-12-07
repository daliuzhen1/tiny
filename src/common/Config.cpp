#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

#include "include/common/Config.h"
std::string Config::getQueryDirWithID(std::string& queryID)
{
    QString path = QCoreApplication::applicationDirPath();
    path += QDir::separator();
    path += queryID.c_str();
    return path.toStdString();
}
std::string Config::getQueryNodeFile(std::string& queryID, std::string nodeID)
{
    QString path = QCoreApplication::applicationDirPath();
    path += QDir::separator();
    path += queryID.c_str();
    path += QDir::separator();
    path += nodeID.c_str();
    path += ".parquet";
    return path.toStdString();
}
std::string Config::getMetaDir()
{
    QString path = QCoreApplication::applicationDirPath();
    path += QDir::separator();
    path += "meta";
    path += QDir::separator();
    return path.toStdString();
}


bool Config::initialize()
{
    if (!QFileInfo(getMetaDir().c_str()).exists())
    {

    }
}
