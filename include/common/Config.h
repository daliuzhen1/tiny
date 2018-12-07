#ifndef Config_H
#define Config_H

#include <string>
class Config
{

public:
    static std::string getQueryDirWithID(std::string& queryID);
    static std::string getQueryNodeFile(std::string& queryID, std::string nodeID);
    static std::string getMetaDir();
    static bool initialize();

};


#endif
