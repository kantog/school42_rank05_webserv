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
    int _cgiFds[3];
    std::vector<std::string> _envStrings;
    std::vector<char *> _env;

    int _statusCode;
    bool _isRunning;

    typedef void (Cgi::*ProcessFunction)(void);
	ProcessFunction _currentFunction;

    void _writeInput(void);
    void _readOutput(void);

    size_t _bytesWritten;

    void _finishCgi(void);

    bool _isWriteDone(const std::string &body);

    bool _checkAccess(void);
    bool _initPipes(void);
    bool _makeNonBlocking(void);
    bool _forkCgi(void);

    void _initEnv(void);
    void _runCgi(void);
    void _setupParentPipes(void);

public:
    Cgi(const HTTPRequest &request, const ServerConfig &serverConfig);
    ~Cgi();

    void startCgi(void);
    bool processCgi(void);

    int getStatusCode() const { return _statusCode; }
    const std::string &getBody() const { return _rawOutput; }
    const int *getCgiFds() const { return _cgiFds; }
};