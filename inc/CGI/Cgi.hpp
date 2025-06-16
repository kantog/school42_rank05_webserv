#pragma once

#include <string>
#include <map>
#include "HTTPRequest.hpp"
#include "ServerConfig.hpp"

class Cgi
{
    private:
        const HTTPRequest &_request;
        const ServerConfig &_serverConfig;

        std::string _path;


        std::string _rawOutput;
	    std::map<std::string, std::string> _headers;
        std::string _body;

        int _statusCode;

        void _parsePath(void);
        void _checkAccess(void);

    public:
        Cgi(const HTTPRequest &request, const ServerConfig &serverConfig);

        
        void run();
        int getStatusCode() const { return _statusCode; }
        const std::string &getBody() const { return _body; }
};