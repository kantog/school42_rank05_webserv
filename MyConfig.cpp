

#include "MyConfig.hpp"
#include "ConfigParser.hpp"

MyConfig::MyConfig(const std::string &filename)
{
    ConfigParser parser;
    _servers = parser.parseConfigFile(filename);
}

// <<singleton>>

MyConfig::~MyConfig() {}