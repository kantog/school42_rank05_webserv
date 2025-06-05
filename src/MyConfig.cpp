

#include "../inc/ConfigParser.hpp"
#include "../inc/MyConfig.hpp"

MyConfig::MyConfig() {}
MyConfig::~MyConfig() {}

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

//////////////////////////////////////////////////////////////////////////
std::string ServerConfig::getServerKey(void) const
{
    std::stringstream ss;
    ss << host << ":" << port;
    return ss.str();
}