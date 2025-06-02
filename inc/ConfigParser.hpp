


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

    std::vector<std::string> _tokens;
    size_t _crurentToken;

private:

    bool hasMoreTokens(void);
    std::string getCurrentToken(void);
    std::string getNextToken(void);
    void expectToken(const std::string& token);

    bool setServerName(ServerConfig& server, const std::string& token);
    bool setListen(ServerConfig& server, const std::string& token);

    void tokenize(std::ifstream& file);
    void parseServer(void);
};