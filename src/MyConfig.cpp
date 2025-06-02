

#include "../inc/ConfigParser.hpp"
#include "../inc/MyConfig.hpp"

MyConfig::MyConfig(const char *filename)
{
    ConfigParser parser;
    _servers = parser.parseConfigFile(std::string(filename));
}

MyConfig& MyConfig::get(const char* filename)
{
    static MyConfig instance(filename);
    return instance;
}

MyConfig::~MyConfig() {}