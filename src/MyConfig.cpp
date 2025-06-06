

#include "../inc/config_classes/ConfigParser.hpp"
#include "../inc/config_classes/MyConfig.hpp"

MyConfig::MyConfig()
{ }

MyConfig::MyConfig(const char *filename)
{
    ConfigParser parser;
    _servers = parser.parseConfigFile(std::string(filename));
}

const MyConfig& MyConfig::get(const char* filename)
{
    static MyConfig instance(filename);
    return instance;
}

int MyConfig::getPort(int serverNumber)
{
    return MyConfig::get()._servers[serverNumber].port;
}

MyConfig::~MyConfig() {}
