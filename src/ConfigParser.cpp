
#include "ConfigParser.hpp"

bool ConfigParser::hasMoreTokens(void)
{
    return this->_crurentToken < this->_tokens.size();
}

std::string ConfigParser::getCurrentToken(void)
{
    return this->_tokens[this->_crurentToken];
}

std::string ConfigParser::getNextToken(void)
{
    if (!hasMoreTokens())
        throw std::runtime_error("No more tokens");
    return this->_tokens[this->_crurentToken++];
}

void ConfigParser::expectToken(const std::string &token)
{
    if (!hasMoreTokens() || getCurrentToken() != token)
        throw std::runtime_error("Expected token: " + token + ", got: " + getCurrentToken());
    this->_crurentToken++;
}

bool ConfigParser::setServerName(ServerConfig &server, const std::string &token)
{
    if (token == "server_name")
    {
        server.host = getNextToken();
        return true;
    }
    return false;
}

void ConfigParser::tokenize(std::ifstream &file)
{
    std::string line;
    while (std::getline(file, line))
    {
        size_t comment_pos = line.find('#');
        if (comment_pos != std::string::npos)
            line = line.substr(0, comment_pos);

        std::istringstream line_stream(line);
        std::string token;
        while (line_stream >> token)
            this->_tokens.push_back(token);
    }
}

std::vector<ServerConfig> ConfigParser::parseConfigFile(const std::string &filename)
{
    std::ifstream file(filename.c_str());
    if (!file.is_open())
        throw std::runtime_error("Cannot open config file: " + filename);
    this->tokenize(file);
    file.close();

    this->_crurentToken = 0;
    while (this->_crurentToken < this->_tokens.size())
    {
        this->parseServer();
    }

    return _configs;
}

void ConfigParser::parseServer()
{
    ServerConfig server;

    this->expectToken("server");
    this->expectToken("{");

    while (hasMoreTokens() && getCurrentToken() != "}")
    {
        std::string token = getNextToken();

        if (this->setServerName(server, token))
            continue;
        else if (this->setListen(server, token))
            continue;
        else
        {
            throw std::runtime_error("Unknown server directive: " + token);
        }
    }

    expectToken("}");
    _configs.push_back(server);
}

void ConfigParser::printConfig(void)
{
    std::cout << "printConfig" << std::endl;
}
