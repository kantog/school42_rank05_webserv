#include "Cgi.hpp"
#include "Defines.hpp"

#include <sys/stat.h>
#include <sstream>
#include <cerrno>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <sys/wait.h>
#include <fcntl.h>

static void closeFd(int *fd)
{
    if (*fd == -1)
        return;
    close(*fd);
    *fd = -1;
}

static void closePipe(int *pipe)
{
    closeFd(&pipe[0]);
    closeFd(&pipe[1]);
}

Cgi::Cgi(const HTTPRequest &request, const ServerConfig &serverConfig) : _request(request),
                                                                         _serverConfig(serverConfig),
                                                                         _statusCode(HTTP_OK)
{
    _path = _serverConfig.getFullFilesystemPath(_request.getRequestTarget());
    _pipeIn[0] = -1;
    _pipeIn[1] = -1;
    _pipeOut[0] = -1;
    _pipeOut[1] = -1;
    _pid = -1;
    _isRunning = false;
    _bytesWritten = 0;
}

Cgi::~Cgi()
{
    closePipe(_pipeIn);
    closePipe(_pipeOut);
    if (_pid > 0)
        kill(_pid, SIGKILL);
}

bool Cgi::_checkAccess()
{
    struct stat sb;
    if (stat(_path.c_str(), &sb) == 0)
    {
        if (S_ISREG(sb.st_mode) && (sb.st_mode & S_IXUSR))
            _statusCode = HTTP_OK;
        else
            _statusCode = HTTP_FORBIDDEN; // script not executable
    }
    else
        _statusCode = HTTP_NOTFOUND; // script not found
    return _statusCode == HTTP_OK;
}

bool Cgi::_initPipes()
{
    if (pipe(_pipeIn) < 0)
        _statusCode = HTTP_SERVER_ERROR; // internal error
    if (pipe(_pipeOut) < 0)
    {
        closePipe(_pipeIn);
        _statusCode = HTTP_SERVER_ERROR; // internal error
    }
    return _statusCode == HTTP_OK;
}

bool Cgi::_makeNonBlocking()
{
    if (fcntl(_pipeIn[0], F_SETFL, O_NONBLOCK) < 0 ||
        fcntl(_pipeOut[1], F_SETFL, O_NONBLOCK) < 0)
        _statusCode = HTTP_SERVER_ERROR; // internal error
    return _statusCode == HTTP_OK;
}

bool Cgi::_forkCgi()
{
    _pid = fork();
    if (_pid < 0)
        _statusCode = HTTP_SERVER_ERROR;
    return _statusCode == HTTP_OK;
}

static std::string sanitizeEnvValue(const std::string &value)
{
    std::string sanitized;
    for (std::string::const_iterator it = value.begin(); it != value.end(); ++it)
    {
        char c = *it;
        if (std::isprint(c) && c != '\n' && c != '\r' && c != '\0')
            sanitized += c;
        else
            sanitized += '_';
    }
    return sanitized;
}

void Cgi::_pushEnv(const std::string &key, const std::string &value)
{
    _envStrings.push_back(key + "=" + sanitizeEnvValue(value));
}

void Cgi::_initEnv()
{
    _pushEnv("HTTP_COOKIE", _request.getHeader("Cookie"));
    _pushEnv("GATEWAY_INTERFACE", "CGI/1.1");
    _pushEnv("REQUEST_METHOD", _request.getMethod());
    _pushEnv("SERVER_PROTOCOL", _request.getVersion());
    _pushEnv("SCRIPT_FILENAME", _path);
    _pushEnv("PATH_INFO", _request.getPathInfo());
    _pushEnv("SERVER_NAME", _request.getHeader("Host"));
    _pushEnv("HTTP_HOST", _request.getHeader("Host"));
    _pushEnv("SERVER_PORT", _serverConfig.port);
    _pushEnv("REMOTE_ADDR", _serverConfig.host);
    _pushEnv("HTTP_USER_AGENT", _request.getHeader("User-Agent"));
    _pushEnv("HTTP_ACCEPT", _request.getHeader("Accept"));
    _pushEnv("REDIRECT_STATUS", "200"); // for php

    if (_request.getMethod() == "GET")
    {
        _pushEnv("QUERY_STRING", _request.getQuery());
        _pushEnv("CONTENT_LENGTH", "");
        _pushEnv("CONTENT_TYPE", "");
    }
    else if (_request.getMethod() == "POST")
    {
        std::stringstream ss;
        ss << _request.getBody().size();
        _pushEnv("CONTENT_LENGTH", ss.str());
        _pushEnv("CONTENT_TYPE", _request.getHeader("Content-Type"));
        _pushEnv("QUERY_STRING", "");
    }

    _env.reserve(_envStrings.size() + 1);
    for (size_t i = 0; i < _envStrings.size(); ++i)
    {
        _env.push_back(const_cast<char *>(_envStrings[i].c_str()));
    }
    _env.push_back(NULL);
}

void Cgi::_runCgi()
{
    if (dup2(_pipeIn[0], STDIN_FILENO) < 0)
    {
        std::cerr << "dup2 stdin failed: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    if (dup2(_pipeOut[1], STDOUT_FILENO) < 0)
    {
        std::cerr << "dup2 stdout failed: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    closePipe(_pipeIn);
    closePipe(_pipeOut);

    std::string interpreter = _serverConfig.getCgiInterpreter(_path);
    if (interpreter.empty()) // normally this should never happen
    {
        std::cerr << "No interpreter found for: " << _path << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string relPath = Path(_path)
                              .removePrefix(_serverConfig.getDocumentRoot().makeRelative())
                              .toString();

    if (!relPath.empty() && relPath[0] == '/')
        relPath = relPath.substr(1);

    char *argv[] = {
        const_cast<char *>(interpreter.c_str()),
        const_cast<char *>(relPath.c_str()),
        NULL};

    chdir(_serverConfig.getDocumentRoot().makeAbsolute().c_str());
    _path = relPath;

    this->_initEnv();
    execve(interpreter.c_str(), argv, _env.data());

    std::cerr << "execve failed: " << strerror(errno) << std::endl;
    exit(EXIT_FAILURE);
}

void Cgi::_setupParentPipes()
{
    closeFd(&_pipeIn[0]);
    closeFd(&_pipeOut[1]);

    _cgiFds[0] = _pipeOut[0];
    _cgiFds[1] = 0;
    _cgiFds[2] = 0;
    if (_request.getMethod() == "POST")
    {
        const std::string &body = _request.getBody();
        if (body.size() <= 4096)
        {
            ssize_t written = write(_pipeIn[1], body.c_str(), body.size());
            if (written == static_cast<ssize_t>(body.size())) // everything written
            {
                closeFd(&_pipeIn[1]);
                this->_currentFunction = &Cgi::_readOutput;
            }
            else
            {
                _cgiFds[1] = _pipeIn[1];
                _bytesWritten = (written > 0) ? written : 0;
                this->_currentFunction = &Cgi::_writeInput;
            }
        }
        else
        {
            _cgiFds[1] = _pipeIn[1];
            this->_currentFunction = &Cgi::_writeInput;
        }
    }
    else
    {
        closeFd(&_pipeIn[1]);
        this->_currentFunction = &Cgi::_readOutput;
    }
    _isRunning = true;
}

bool Cgi::_isWriteDone(const std::string &body)
{
    if (_bytesWritten >= body.size())
    {
        closeFd(&_pipeIn[1]);
        _currentFunction = &Cgi::_readOutput;
        return true;
    }
    return false;
}

void Cgi::_writeInput()
{
    const std::string &body = _request.getBody();

    if (_isWriteDone(body))
        return;

    int status;
    if (waitpid(_pid, &status, WNOHANG) > 0)
    {
        std::cerr << "CGI child process died before we could write all data" << std::endl;
        _statusCode = HTTP_SERVER_ERROR;
        closeFd(&_pipeIn[1]);
        _currentFunction = &Cgi::_readOutput;
        return;
    }

    ssize_t bytesToWrite = body.size() - _bytesWritten;
    ssize_t written = write(_pipeIn[1], body.c_str() + _bytesWritten, bytesToWrite);
    if (written < 0)
    {
        std::cerr << "CGI write error: " << strerror(errno) << std::endl;
        _statusCode = HTTP_SERVER_ERROR;
        closeFd(&_pipeIn[1]);
        _currentFunction = &Cgi::_readOutput;
        return;
    }
    _bytesWritten += written;
    _isWriteDone(body);
}

void Cgi::_readOutput()
{
    char buffer[16384];
    ssize_t bytesRead = read(_pipeOut[0], buffer, sizeof(buffer));

    if (bytesRead < 0)
    {
        std::cerr << "CGI read error: " << strerror(errno) << std::endl;
        _statusCode = HTTP_SERVER_ERROR;
        closeFd(&_pipeOut[0]);
        _finishCgi();
        return;
    }

    if (bytesRead == 0)
    {
        closeFd(&_pipeOut[0]);
        _finishCgi();
        return;
    }
    _rawOutput.append(buffer, bytesRead);
}

void Cgi::_finishCgi()
{
    int status;
    pid_t result = waitpid(_pid, &status, 0); // WNOHANG

    if (result > 0) // child is done
    {
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
        {
            _statusCode = HTTP_SERVER_ERROR;
            std::cerr << "CGI child process exited with status " << WEXITSTATUS(status) << std::endl;
        }
    }
    else
        _statusCode = HTTP_SERVER_ERROR;
    _isRunning = false;
}

void Cgi::startCgi()
{

    if (!this->_checkAccess() ||
        !this->_initPipes() ||
        !this->_makeNonBlocking() ||
        !this->_forkCgi())
        return;

    if (_pid == 0)
        this->_runCgi();

    int status; // if child is already dead
    if (waitpid(_pid, &status, WNOHANG) > 0)
    {
        _statusCode = HTTP_SERVER_ERROR;
        return;
    }

    this->_setupParentPipes();
}

bool Cgi::processCgi()
{
    if (isOverTimeLimit())
    {
        _statusCode = HTTP_GATEWAY_TIMEOUT;
        _isRunning = false;
        return false;
    }

    (this->*_currentFunction)();

    if (_statusCode != HTTP_OK)
        return false;
    return _isRunning;
}
