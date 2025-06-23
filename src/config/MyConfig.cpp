

#include "../../inc/config_classes/ConfigParser.hpp"
#include "../../inc/config_classes/MyConfig.hpp"

MyConfig *MyConfig::_myConfig = NULL;

MyConfig::MyConfig() {}

MyConfig::~MyConfig() 
{
	if (_myConfig)
		delete _myConfig;
}

MyConfig::MyConfig(const char *filename)
{
    ConfigParser parser;
    _servers = parser.parseConfigFile(std::string(filename));
}

const MyConfig& MyConfig::get(const char* filename)
{
	if (!_myConfig)
		_myConfig = new MyConfig(filename);
    return *_myConfig;
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
