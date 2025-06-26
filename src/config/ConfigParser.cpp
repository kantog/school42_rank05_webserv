
#include "../../inc/config_classes/ConfigParser.hpp"

#include <stdexcept> // std::runtime_error
#include <iostream>  // std::cout
#include <fstream>   // std::ifstream
#include <sstream>   // std::stringstream
#include <cstdlib>

bool ConfigParser::hasMoreTokens(void)
{
    return this->_curentToken < this->_tokens.size();
}

std::string ConfigParser::getCurrentToken(void)
{
    return this->_tokens[this->_curentToken];
}

std::string ConfigParser::getNextToken(void)
{
    if (!hasMoreTokens())
        throw std::runtime_error("No more tokens");
    return this->_tokens[this->_curentToken++];
}

void ConfigParser::expectToken(const std::string &token)
{
    if (!hasMoreTokens() || getCurrentToken() != token)
        throw std::runtime_error("Expected token: " + token + ", got: " + getCurrentToken());
    this->_curentToken++;
}

bool ConfigParser::setServerName(ServerConfig &server, const std::string &token)
{
    if (token != "server_name")
        return false;

    std::string name = getNextToken();
    while (name != ";")
    {
        server.server_names.push_back(name);
        name = getNextToken();
    }
    return true;
}

bool ConfigParser::setListen(ServerConfig &server, const std::string &token)
{
    if (token != "listen")
        return false;
    std::string listen_value = getNextToken();
    size_t colon_pos = listen_value.find(':');

    if (colon_pos != std::string::npos)
    {
        server.host = listen_value.substr(0, colon_pos);
        server.port = listen_value.substr(colon_pos + 1);
    }
    else
    {
        server.port = listen_value;
        server.host = "127.0.0.1";
    }
    expectToken(";");
    return true;
}

bool ConfigParser::setClientMaxBodySize(ServerConfig &server, const std::string &token)
{
    if (token != "client_max_body_size")
        return false;
    server.client_max_body_size = std::atoi(getNextToken().c_str());
    expectToken(";");
    return true;
}

bool ConfigParser::setErrorPage(ServerConfig &server, const std::string &token)
{
    if (token != "error_page")
        return false;

    /* vb:
    error_page 404 /errors/404.html;
    error_page 404 502 503 504 /errors/50x.html;
    */
    std::vector<int> error_codes;

    // 404
    error_codes.push_back(std::atoi(getNextToken().c_str()));
    // 502/file
    std::string current_error_token = getNextToken();
    // 503/
    std::string next_error_token = getNextToken();

    while (next_error_token != ";")
    {
        error_codes.push_back(std::atoi(current_error_token.c_str()));
        current_error_token = next_error_token;
        next_error_token = getNextToken();
    }
    for (size_t i = 0; i < error_codes.size(); i++)
    {
        server.error_pages[error_codes[i]] = current_error_token;
    }
    return true;
}

bool ConfigParser::setRoot(ServerConfig &server, const std::string &token)
{
    if (token != "root")
        return false;
    server.root = getNextToken();
    expectToken(";");
    return true;
}

bool ConfigParser::setClientMaxBodySize(Route &route, const std::string &token)
{
    if (token != "client_max_body_size")
        return false;
    route.client_max_body_size = std::atoi(getNextToken().c_str());
    expectToken(";");
    return true;
}

bool ConfigParser::setRoot(Route &route, const std::string &token)
{
    if (token != "root")
        return false;
    route.root = getNextToken();
    expectToken(";");
    return true;
}

bool ConfigParser::setAllowedMethods(Route &route, const std::string &token)
{
    if (token != "allowed_methods")
        return false;
    std::string methods = getNextToken();
    while (methods != ";")
    {
        route.allowedMethods.push_back(methods);
        methods = getNextToken();
    }
    return true;
}

bool ConfigParser::setIndex(Route &route, const std::string &token)
{
    if (token != "index")
        return false;

    std::string file = getNextToken();
    while (file != ";")
    {
        route.defaultFiles.push_back(file);
        file = getNextToken();
    }
    return true;
}

bool ConfigParser::setAutoIndex(Route &route, const std::string &token)
{
    if (token != "autoindex")
        return false;
    route.isDirectoryListing = getNextToken() == "on";
    expectToken(";");
    return true;
}

bool ConfigParser::setAllowUpload(Route &route, const std::string &token)
{
    if (token != "allow_upload")
        return false;
    route.uploadAllowed = getNextToken() == "on";
    expectToken(";");
    return true;
}

bool ConfigParser::setUploadPath(Route &route, const std::string &token)
{
    if (token != "upload_path")
        return false;
    route.uploadPath = getNextToken();
    expectToken(";");
    return true;
}

bool ConfigParser::setCgi(Route &route, const std::string &token)
{
    if (token != "cgi_extension")
        return false;

    std::string extension = getNextToken();
    std::string path = getNextToken();

    while (extension != ";" && path != ";")
    {
        route.cgiExtensions[extension] = path;

        if (hasMoreTokens() && getCurrentToken() != ";")
        {
            extension = getNextToken();
            if (extension != ";")
                path = getNextToken();
        }
        else
            break;
    }

    if (hasMoreTokens() && getCurrentToken() == ";")
        expectToken(";");

    return true;
}

bool ConfigParser::setReturn(Route &route, const std::string &token)
{
    if (token != "return")
        return false;

    route.redirectCode = std::atoi(getNextToken().c_str());
    route.redirectPath = getNextToken();
    expectToken(";");
    return true;
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
        else if (this->setClientMaxBodySize(server, token))
            continue;
        else if (this->setErrorPage(server, token))
            continue;
        else if (this->setRoot(server, token))
            continue;
        else if (this->parseLocation(server, token))
            continue;
        else
        {
            throw std::runtime_error("Unknown server directive: " + token);
        }
    }

    expectToken("}");
    this->validateServer(server);
    _configs[server.getServerKey()].push_back(server);
}

bool ConfigParser::parseLocation(ServerConfig &server, const std::string &name)
{
    if (name != "location")
        return false;

    Route route;

    route.path = getNextToken();
    this->getNextToken(); // skip {

    while (hasMoreTokens() && getCurrentToken() != "}")
    {
        std::string token = getNextToken();

        if (this->setRoot(route, token))
            continue;
        else if (this->setAllowedMethods(route, token))
            continue;
        else if (this->setIndex(route, token))
            continue;
        else if (this->setAutoIndex(route, token))
            continue;
        else if (this->setAllowUpload(route, token))
            continue;
        else if (this->setUploadPath(route, token))
            continue;
        else if (this->setCgi(route, token))
            continue;
        else if (this->setReturn(route, token))
            continue;
        else if (this->setClientMaxBodySize(route, token))
            continue;
        else
        {
            throw std::runtime_error("Unknown location directive: " + token);
        }
    }
    this->expectToken("}");
    this->validateRoutes(route);
    server.routes.push_back(route);
    return true;
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
        {
            if (!token.empty() && token[token.size() - 1] == ';')
            {
                this->_tokens.push_back(token.substr(0, token.size() - 1));
                this->_tokens.push_back(";");
            }
            else
                this->_tokens.push_back(token);
        }
    }
}

void ConfigParser::validateRoutes(Route &route)
{
    if (route.path.empty())
        throw std::runtime_error("Route path is empty");
    // if (route.allowedMethods.empty())
    //     throw std::runtime_error("Route allowed methods is empty");
}

void ConfigParser::validateServer(ServerConfig &server)
{
    if (server.server_names.empty())
        throw std::runtime_error("Server name is empty");
    if (server.host.empty())
        throw std::runtime_error("Server listen is empty");
    if (server.client_max_body_size == 0)
        throw std::runtime_error("Server client_max_body_size is empty");
    if (server.routes.empty())
        throw std::runtime_error("Server has no routes");
}

void ConfigParser::validateParsedConfig(void)
{
    if (this->_configs.empty())
        throw std::runtime_error("No servers found in config file");
}

std::map<std::string, std::vector<ServerConfig> > ConfigParser::parseConfigFile(const std::string &filename)
{
    if (filename.empty())
        throw std::runtime_error("Config file name is empty");
    if (filename.find(".conf") == std::string::npos)
        throw std::runtime_error("Config file must have .conf extension");
    std::ifstream file(filename.c_str());
    if (!file.is_open())
        throw std::runtime_error("Cannot open config file: " + filename);
    this->tokenize(file);
    file.close();

    this->_curentToken = 0;
    while (this->_curentToken < this->_tokens.size())
    {
        this->parseServer();
    }

    this->validateParsedConfig();
    return _configs;
}
