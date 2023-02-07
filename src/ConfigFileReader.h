#ifndef CONFIG_FILE_READER_H
#define CONFIG_FILE_READER_H

#include <string>
#include <iostream>
#include <boost/property_tree/xml_parser.hpp>

#include "ProjectConfig.h"

namespace BlockchainIndexer
{

static const std::string TOP_PROFILE = "BlockchainIndexer";

class ConfigFileReader
{
public:
    ConfigFileReader();
    ~ConfigFileReader();

    bool init(std::string& aFilename);
    bool getParameter(const std::string aPropertyKey, std::string& aProperty);
    bool getParameter(const std::string aPropertyKey, int& aProperty);
    bool getParameter(const std::string aPropertyKey, float& aProperty);
    bool getParameter(const std::string aPropertyKey, double& aProperty);
    bool getParameter(const std::string aPropertyKey, bool& aProperty);
    bool getConfiguration(ProjectConfig& aConfig);

private:
    std::string fileName;
    std::string topLevelProfile;
    boost::property_tree::ptree mPropertyTree;
};

}

#endif  // CONFIG_FILE_READER_H