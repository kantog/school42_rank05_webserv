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

    int _pipeIn[2];
    int _pipeOut[2];
    int _pid;
    std::vector<std::string> _envStrings;
    std::vector<char *> _env;

    int _statusCode;

    bool _checkAccess(void);
    bool _initPipes(void);
    bool _forkCgi(void);

    void _initEnv(void);
    void _runCgi(void);
    void lol(void);

public:
    Cgi(const HTTPRequest &request, const ServerConfig &serverConfig);

    void startCgi();
    void readOutput();

    int getStatusCode() const { return _statusCode; }
    const std::string &getBody() const { return _rawOutput; }
};