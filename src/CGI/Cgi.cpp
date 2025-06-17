

#include "Cgi.hpp"

#include <sys/stat.h>
#include <sstream>
#include <cerrno>

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
// TODO: check list

/*
| Variable          | Betekenis                                                                     |
| ----------------- | ----------------------------------------------------------------------------- |
| `REQUEST_METHOD`  | HTTP-method, zoals `GET` of `POST`                                            |
| `QUERY_STRING`    | Alles na `?` in de URL, bv `name=John&age=22`                                 |
| `CONTENT_TYPE`    | Voor POST: bijv. `application/x-www-form-urlencoded` of `multipart/form-data` |
| `CONTENT_LENGTH`  | Lengte van de body (voor POST/PUT)                                            |
| `SCRIPT_FILENAME` | Volledig pad naar het script op de schijf                                     |
| `PATH_INFO`       | Deel van URL na het script pad                                                |
| `SERVER_PROTOCOL` | Protocol zoals `HTTP/1.1`                                                     |
| `HTTP_HOST`       | Hostnaam uit de request header                                                |
| `HTTP_USER_AGENT` | User-Agent header van de client                                               |
| `HTTP_COOKIE`     | Cookies van de client                                                         |
| `REMOTE_ADDR`     | IP van de client                                                              |


📦 Voorbeeld: GET request naar /cgi/test.py/foo/bar?name=jan
Stel je configuratie zegt:

conf
Kopiëren
Bewerken
location /cgi {
    root /home/user/webroot;
    cgi_extension .py /usr/bin/python3;
}
Request:

pgsql
Kopiëren
Bewerken
GET /cgi/test.py/foo/bar?name=jan HTTP/1.1
Host: localhost:8080
User-Agent: curl/7.79.1
Jouw server moet het CGI-script /home/user/webroot/test.py starten en deze environment meegeven:

env
Kopiëren
Bewerken
REQUEST_METHOD=GET
QUERY_STRING=name=jan
CONTENT_LENGTH=
CONTENT_TYPE=
SCRIPT_FILENAME=/home/user/webroot/test.py
PATH_INFO=/foo/bar
SERVER_PROTOCOL=HTTP/1.1
HTTP_HOST=localhost:8080
HTTP_USER_AGENT=curl/7.79.1
🧪 Voorbeeld: POST request met form
Request:

makefile
Kopiëren
Bewerken
POST /cgi/upload.php HTTP/1.1
Host: localhost
Content-Type: application/x-www-form-urlencoded
Content-Length: 20

name=jan&city=gent
Dan stel je in:

env
Kopiëren
Bewerken
REQUEST_METHOD=POST
CONTENT_TYPE=application/x-www-form-urlencoded
CONTENT_LENGTH=20
SCRIPT_FILENAME=/home/user/webroot/upload.php
PATH_INFO=
SERVER_PROTOCOL=HTTP/1.1
HTTP_HOST=localhost
En de POST-body (name=jan&city=gent) stuur je via stdin naar het CGI-proces.
*/

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
    _path = _serverConfig.getFullPath(_request.getRequestTarget());
    _pipeIn[0] = -1;
    _pipeIn[1] = -1;
    _pipeOut[0] = -1;
    _pipeOut[1] = -1;
    _pid = -1;
    _isRunning = false;
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
            _statusCode = 403;
    }
    else
        _statusCode = 404;
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
    // TODO: check list
    // TODO: coocies
    _envStrings.push_back("GATEWAY_INTERFACE=CGI/1.1");
    _envStrings.push_back("REQUEST_METHOD=" + _request.getMethod());
    _envStrings.push_back("SERVER_PROTOCOL=" + _request.getVersion());
    _envStrings.push_back("SCRIPT_FILENAME=" + _request.getRequestFile());
    _envStrings.push_back("PATH_INFO=" + _request.getPathInfo());

    _envStrings.push_back("HTTP_HOST=" + _request.getHeader("Host"));
    _envStrings.push_back("HTTP_USER_AGENT=" + _request.getHeader("User-Agent"));

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
    if (interpreter.empty())
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

// TODO: fiks name
void Cgi::lol()
{
    closeFd(&_pipeIn[0]);
    closeFd(&_pipeOut[1]);

    if (_request.getMethod() == "POST")
        this->_currentFunction = &Cgi::_writeInput;
    else
    {
        closeFd(&_pipeIn[1]);
        this->_currentFunction = &Cgi::_readOutput;
    }
    _isRunning = true; // TODO ?
    // if (_request.getMethod() == "POST" && !_request.getBody().empty())
    //     write(_pipeIn[1], _request.getBody().c_str(), _request.getBody().size());

    // close(_pipeIn[1]);

    // this->readOutput(); // TODO: naar eepol
    // int status;
    // waitpid(_pid, &status, 0);
    // if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
    //     _statusCode = 500;
}

bool Cgi::_isWriteDone(const std::string &body)
{
    if (_bytesWritten >= body.size())
    {
        closeFd(&_pipeIn[0]);

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

    ssize_t bytesToWrite = body.size() - _bytesWritten;
    ssize_t written = write(_pipeIn[1], body.c_str() + _bytesWritten, bytesToWrite);

    if (written < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return; // more data to write
        std::cerr << "CGI write error: " << strerror(errno) << std::endl;
        _statusCode = 500;
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

    if (result > 0)
    {
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
            _statusCode = 500;
    }
    // else if (result == 0) TODO Zombie
    //     // save pis?
    else
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
    this->lol();
}

bool Cgi::processCgi()
{
    (this->*_currentFunction)();

    if (_statusCode != 200)
        return false;
    return _isRunning;
}

/* server:
if (!processCgi())
{
    handelr.procesCGi()
    removeCgi();
    return;
}

handelr.procesCGi()
{
    if (_statusCode != 200)
        buildErrorPage();
    else
        buildCgiPage();
}

removeCgi()
{
    verijd uit eepl
    verwijder cgi
    verwijder map
}
*/