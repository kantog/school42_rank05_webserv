


# pragma once

# include "MyConfig.hpp"

class ConfigParser
{
public:

    ConfigParser() {}
    ~ConfigParser() {}

    std::vector<ServerConfig> parseConfigFile(const std::string& filename);

private:

    std::vector<ServerConfig> _configs;

    std::vector<std::string> _tokens;
    size_t _curentToken;

private:

    bool hasMoreTokens(void);
    std::string getCurrentToken(void);
    std::string getNextToken(void);
    void expectToken(const std::string& token);

    bool setServerName(ServerConfig& server, const std::string& token);
    bool setListen(ServerConfig& server, const std::string& token);
    bool setClientMaxBodySize(ServerConfig& server, const std::string& token); // clientMaxBodySize
    bool setErrorPage(ServerConfig& server, const std::string& token);

    bool parseLocation(ServerConfig& server, const std::string& token);
    bool setRoot(Route& route, const std::string& token);
    bool setAllowedMethods(Route& route, const std::string& token);
    bool setIndex(Route& route, const std::string& token);
    bool setAutoIndex(Route& route, const std::string& token);
    bool setAllowUpload(Route& route, const std::string& token);
    bool setUploadPath(Route& route, const std::string& token);
    bool setCgi(Route& route, const std::string& token);

    void tokenize(std::ifstream& file);
    void parseServer(void);
};