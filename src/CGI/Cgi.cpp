#include "Cgi.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>


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
                                                                         _statusCode(200)
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
    /* TODO:
    if (_pid > 0) {
    kill(_pid, SIGTERM);  // Try graceful termination first

    // Give it a moment to terminate gracefully
    int status;
    if (waitpid(_pid, &status, WNOHANG) == 0) {
        // Still running, force kill
        std::cerr << "CGI process " << _pid << " didn't terminate gracefully, killing..." << std::endl;
        kill(_pid, SIGKILL);
        waitpid(_pid, &status, 0); // Wait for it to die
    }
    _pid = -1;

    of
        if (_pid > 0) {
        kill(_pid, SIGTERM);  // Try graceful termination first

        // Give it a moment to terminate gracefully
        int status;
        struct timespec ts = {0, 100000000}; // 100ms
        nanosleep(&ts, NULL);

        if (waitpid(_pid, &status, WNOHANG) == 0) {
            // Still running, force kill
            kill(_pid, SIGKILL);
            waitpid(_pid, &status, 0);
        }
        _pid = -1;
    }
}
    */
}

bool Cgi::_checkAccess()
{
    struct stat sb;
    if (stat(_path.c_str(), &sb) == 0)
    {
        if (S_ISREG(sb.st_mode) && (sb.st_mode & S_IXUSR))
            _statusCode = 200; 
        else
            _statusCode = 500; // script not executable
    }
    else
        _statusCode = 404; // script not found
    return _statusCode == 200;
}

bool Cgi::_initPipes()
{
    if (pipe(_pipeIn) < 0)
        _statusCode = 500;
    if (pipe(_pipeOut) < 0)
    {
        closePipe(_pipeIn);
        _statusCode = 500;
    }
    return _statusCode == 200;
}

bool Cgi::_makeNonBlocking()
{
    if (fcntl(_pipeIn[0], F_SETFL, O_NONBLOCK) < 0 ||
        fcntl(_pipeOut[1], F_SETFL, O_NONBLOCK) < 0)
        _statusCode = 500;
    return _statusCode == 200;
}

bool Cgi::_forkCgi()
{
    _pid = fork();
    if (_pid < 0)
        _statusCode = 500;
    return _statusCode == 200;
}

void Cgi::_initEnv()
{
    // TODO: sanitization?
    _envStrings.push_back("HTTP_COOKIE=" + _request.getHeader("Cookie"));
    _envStrings.push_back("GATEWAY_INTERFACE=CGI/1.1");
    _envStrings.push_back("REQUEST_METHOD=" + _request.getMethod());
    _envStrings.push_back("SERVER_PROTOCOL=" + _request.getVersion());
    _envStrings.push_back("SCRIPT_FILENAME=" + _path);
    _envStrings.push_back("PATH_INFO=" + _request.getPathInfo());
    _envStrings.push_back("SERVER_NAME=" + _request.getHeader("Host"));
    _envStrings.push_back("HTTP_HOST=" + _request.getHeader("Host"));
    _envStrings.push_back("SERVER_PORT=" + _serverConfig.port);
    _envStrings.push_back("REMOTE_ADDR=" + _serverConfig.host);
    _envStrings.push_back("HTTP_USER_AGENT=" + _request.getHeader("User-Agent"));
    _envStrings.push_back("HTTP_ACCEPT=" + _request.getHeader("Accept"));


    _envStrings.push_back("REDIRECT_STATUS=200"); // for php

    if (_request.getMethod() == "GET")
    {
        _envStrings.push_back("QUERY_STRING=" + _request.getQuery());
        _envStrings.push_back("CONTENT_LENGTH=");
        _envStrings.push_back("CONTENT_TYPE=");
    }
    else if (_request.getMethod() == "POST")
    {
        std::stringstream ss;
        ss << _request.getBody().size();
        _envStrings.push_back("CONTENT_LENGTH=" + ss.str());

        _envStrings.push_back("CONTENT_TYPE=" + _request.getHeader("Content-Type"));
        _envStrings.push_back("QUERY_STRING=");
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

    this->_initEnv();
    std::string interpreter = _serverConfig.getCgiInterpreter(_path);
    if (interpreter.empty()) // normally this should never happen
    {
        std::cerr << "No interpreter found for: " << _path << std::endl;
        exit(EXIT_FAILURE);
    }
    char *argv[] = {
        const_cast<char *>(interpreter.c_str()),
        const_cast<char *>(_path.c_str()),
        NULL};

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
        _statusCode = 500;
        closeFd(&_pipeIn[1]);
        _currentFunction = &Cgi::_readOutput;
        return;
    }

    ssize_t bytesToWrite = body.size() - _bytesWritten;
    ssize_t written = write(_pipeIn[1], body.c_str() + _bytesWritten, bytesToWrite);
    if (written < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return; // more data to write
        std::cerr << "CGI write error: " << strerror(errno) << std::endl;
        _statusCode = 500;
        closeFd(&_pipeIn[1]);
        _currentFunction = &Cgi::_readOutput;
        return;
    }
    _bytesWritten += written;
    _isWriteDone(body);
}

void Cgi::_readOutput()
{
    char buffer[4096];
    ssize_t bytesRead = read(_pipeOut[0], buffer, sizeof(buffer));

    if (bytesRead < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return; // more data to read
        std::cerr << "CGI read error: " << strerror(errno) << std::endl;
        _statusCode = 500;
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
    pid_t result = waitpid(_pid, &status, WNOHANG);

    if (result > 0) // child is done
    {
        // if (!normal exit || exit code != 0)
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
            _statusCode = 500;
    }
    // else if (result == 0) TODO Zombie
    //     // save pids?
    else // error
        _statusCode = 500;
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

    int status; // if child is all dead
    if (waitpid(_pid, &status, WNOHANG) > 0)
    {
        _statusCode = 500;
        return;
    }

    this->_setupParentPipes();
}

bool Cgi::processCgi()
{
    (this->*_currentFunction)();

    std::cout << "CGI status code: " << _statusCode << std::endl;
    if (_statusCode != 200)
        return false;
    return _isRunning;
}
