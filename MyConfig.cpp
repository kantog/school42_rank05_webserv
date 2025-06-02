

#include "MyConfig.hpp"
#include "ConfigParser.hpp"

MyConfig::MyConfig(const std::string &filename)
{
    ConfigParser parser;
    _servers = parser.parseConfigFile(filename);
}

MyConfig& MyConfig::get(const std::string* filename)
{
    static MyConfig instance(*filename);
    return instance;
}

MyConfig::~MyConfig() {}