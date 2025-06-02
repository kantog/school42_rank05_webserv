


# pragma once

# include "MyConfig.hpp"

class ConfigParser
{
public:

    ConfigParser() {}
    ~ConfigParser() {}

    std::vector<ServerConfig> parseConfigFile(const std::string& filename);
    void printConfig(void);

private:

    std::vector<ServerConfig> _configs;


private:

    bool openFile(const std::string& filename);
};