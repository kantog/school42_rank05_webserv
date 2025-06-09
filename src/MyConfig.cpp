

#include "../inc/config_classes/ConfigParser.hpp"
#include "../inc/config_classes/MyConfig.hpp"

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

const ServerConfig* MyConfig::findServerConfig(const std::string &serverKey, const std::string &hostURL) const
{
    // TODO mulies servir names?
    std::map<std::string, std::vector<ServerConfig> >::const_iterator it = _servers.find(serverKey);
    if (it == _servers.end())
        return NULL;
    for (std::vector<ServerConfig>::const_iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
    {
        if (it2->host == hostURL)
            return &(*it2);
    }
    return &(*it->second.begin()); // default to the first
}

const ServerConfig *MyConfig::getServerConfig(const std::string &serverKey, const std::string &hostURL)
{
    return MyConfig::get().findServerConfig(serverKey, hostURL);
}

//////////////////////////////////////////////////////////////////////////
std::string ServerConfig::getServerKey(void) const
{
    std::stringstream ss;
    ss << host << ":" << port;
    return ss.str();
}
