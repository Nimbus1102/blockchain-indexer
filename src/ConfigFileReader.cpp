#include "ConfigFileReader.h"

namespace BlockchainIndexer
{

ConfigFileReader::ConfigFileReader()
    : topLevelProfile(TOP_PROFILE)
{
}

ConfigFileReader::~ConfigFileReader()
{
}

bool ConfigFileReader::init(std::string& aFilename)
{
    fileName = aFilename;
    mPropertyTree.clear();
    bool isSuccessful = true;

    try
    {
        boost::property_tree::xml_parser::read_xml(fileName, mPropertyTree, boost::property_tree::xml_parser::trim_whitespace);
    }
    catch (std::exception& e)
    {
        std::cout << "Error: " << "findConfigurationFiles:" << "LoadConfigFile Failed - " << e.what() << std::endl;
        isSuccessful = false;
    }
    catch (...)
    {
        std::cout << "Error: " << __FUNCTION__ << ": General non-std exception" << std::endl;
        isSuccessful = false;
    }

    return isSuccessful;
}

bool ConfigFileReader::getParameter(const std::string aPropertyKey, std::string& aProperty)
{
    bool isSuccessful = true;
    try
    {
        const std::string PropertyKeyFull = topLevelProfile + "." + aPropertyKey;
        aProperty = mPropertyTree.get<std::string>(PropertyKeyFull);
        std::cout << aPropertyKey << ": " << aProperty << std::endl;
    }
    catch (std::exception& e)
    {
        std::cout << "Error: " << __FUNCTION__ << ": " << aPropertyKey << ": " << aProperty << ": " << e.what() << std::endl;
        isSuccessful = false;
    }
    catch (...)
    {
        std::cout << "Error: " << __FUNCTION__ << ": General non-std exception" << std::endl;
        isSuccessful = false;
    }
    return isSuccessful;
}

bool ConfigFileReader::getParameter(const std::string aPropertyKey, int& aProperty)
{
    bool isSuccessful = true;
    try
    {
        const std::string PropertyKeyFull = topLevelProfile + "." + aPropertyKey;
        aProperty = mPropertyTree.get<int>(PropertyKeyFull);
        std::cout << aPropertyKey << ": " << aProperty << std::endl;
    }
    catch (std::exception& e)
    {
        std::cout << "Error: " << __FUNCTION__ << ": " << aPropertyKey << ": " << aProperty << ": " << e.what() << std::endl;
        isSuccessful = false;
    }
    catch (...)
    {
        std::cout << "Error: " << __FUNCTION__ << ": General non-std exception" << std::endl;
        isSuccessful = false;
    }
    return isSuccessful;
}

bool ConfigFileReader::getParameter(const std::string aPropertyKey, double& aProperty)
{
    bool isSuccessful = true;
    try
    {
        const std::string PropertyKeyFull = topLevelProfile + "." + aPropertyKey;
        aProperty = mPropertyTree.get<double>(PropertyKeyFull);
        std::cout << aPropertyKey << ": " << aProperty << std::endl;
    }
    catch (std::exception& e)
    {
        std::cout << "Error: " << __FUNCTION__ << ": " << aPropertyKey << ": " << aProperty << ": " << e.what() << std::endl;
        isSuccessful = false;
    }
    catch (...)
    {
        std::cout << "Error: " << __FUNCTION__ << ": General non-std exception" << std::endl;
        isSuccessful = false;
    }
    return isSuccessful;
}

bool ConfigFileReader::getParameter(const std::string aPropertyKey, float& aProperty)
{
    bool isSuccessful = true;
    try
    {
        const std::string PropertyKeyFull = topLevelProfile + "." + aPropertyKey;
        aProperty = mPropertyTree.get<float>(PropertyKeyFull);
        std::cout << aPropertyKey << ": " << aProperty << std::endl;
    }
    catch (std::exception& e)
    {
        std::cout << "Error: " << __FUNCTION__ << ": " << aPropertyKey << ": " << aProperty << ": " << e.what() << std::endl;
        isSuccessful = false;
    }
    catch (...)
    {
        std::cout << "Error: " << __FUNCTION__ << ": General non-std exception" << std::endl;
        isSuccessful = false;
    }
    return isSuccessful;
}

bool ConfigFileReader::getParameter(const std::string aPropertyKey, bool& aProperty)
{
    bool isSuccessful = true;
    try
    {
        const std::string PropertyKeyFull = topLevelProfile + "." + aPropertyKey;
        aProperty = mPropertyTree.get<bool>(PropertyKeyFull);
        std::cout << aPropertyKey << ": " << aProperty << std::endl;       
    }
    catch (std::exception& e)
    {
        std::cout << "Error: " << __FUNCTION__ << ": " << aPropertyKey << ": " << aProperty << ": " << e.what() << std::endl;
        isSuccessful = false;
    }
    catch (...)
    {
        std::cout << "Error: " << __FUNCTION__ << ": General non-std exception" << std::endl;
        isSuccessful = false;
    }
    return isSuccessful;
}

bool ConfigFileReader::getConfiguration(ProjectConfig& aConfig)
{
    bool isSuccessful = true;

    // application parameters
    isSuccessful = getParameter("maxBlocks", aConfig.maxBlocks) && isSuccessful;
    isSuccessful = getParameter("publishPeriod", aConfig.publishPeriod) && isSuccessful;
    isSuccessful = getParameter("xConfirmations", aConfig.xConfirmations) && isSuccessful;
    isSuccessful = getParameter("blockchainFile", aConfig.blockchainFile) && isSuccessful;
    isSuccessful = getParameter("databaseDirectory", aConfig.databaseDirectory) && isSuccessful;

    // test parameters
    isSuccessful = getParameter("runTestCases", aConfig.runTestCases) && isSuccessful;
    isSuccessful = getParameter("testCaseNumber", aConfig.testCaseNumber) && isSuccessful;


    return isSuccessful;
}

}