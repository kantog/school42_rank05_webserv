

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

Cgi::Cgi(const HTTPRequest &request, const ServerConfig &serverConfig) : _request(request),
                                                                         _serverConfig(serverConfig),
                                                                         _statusCode(200)
{
    _path = _serverConfig.getFullPath(_request.getRequestTarget());
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
    if (pipe(_pipeIn) < 0 || pipe(_pipeOut) < 0)
        _statusCode = 500;
    return _statusCode == 200;
}

bool Cgi::_forkCgi()
{
    _pid = fork();
    if (_pid < 0)
        _statusCode = 500; // TODO: check
    return _statusCode == 200;
}

void Cgi::_initEnv()
{
    // TODO: check list
    _envStrings.push_back("PATH_INFO=" + _request.getPathInfo());
    _envStrings.push_back("SCRIPT_FILENAME=" + _request.getRequestFile());
    _envStrings.push_back("GATEWAY_INTERFACE=CGI/1.1");
    _envStrings.push_back("REQUEST_METHOD=" + _request.getMethod());
    _envStrings.push_back("SERVER_PROTOCOL=" + _request.getVersion());

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
    this->_initEnv();
    
    std::cerr << "debug: _path: " << _path << std::endl;
    const std::string interpreter = _serverConfig.getCgiInterpreter(_path); // bijv "/usr/bin/python3"
    std::cerr << "debug: interpreter: " << interpreter << std::endl;
    char *argv[] = {
        const_cast<char *>(interpreter.c_str()),  // argv[0] = "/usr/bin/python3"
        const_cast<char *>(_path.c_str()),        // argv[1] = "/home/user/webroot/test.py"
        NULL
    };

    dup2(_pipeIn[0], STDIN_FILENO);
    dup2(_pipeOut[1], STDOUT_FILENO);

    close(_pipeIn[1]);
    close(_pipeOut[0]);

    std::cerr << "debug: Running " << interpreter << " " << _path << "..." << std::endl;
    execve(interpreter.c_str(), argv, _env.data());  // Voer Python uit, niet het script

    // TODO mag die errno?
    std::cerr << "execve failed: " << strerror(errno) << std::endl;
    exit(EXIT_FAILURE);
}

void Cgi::readOutput()
{
    char buffer[1024];
    ssize_t bytes;
    while ((bytes = read(_pipeOut[0], buffer, sizeof(buffer))) > 0)
    {
        _rawOutput.append(buffer, bytes);
    }

    close(_pipeOut[0]);
}

// TODO: fiks name
void Cgi::lol()
{
    close(_pipeIn[0]);
    close(_pipeOut[1]);

    if (_request.getMethod() == "POST" && !_request.getBody().empty())
        write(_pipeIn[1], _request.getBody().c_str(), _request.getBody().size());

    close(_pipeIn[1]);

    this->readOutput(); // TODO: naar eepol
    int status;
    waitpid(_pid, &status, 0);
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
        _statusCode = 500;
}

void Cgi::startCgi()
{
    if (!this->_checkAccess() ||
        !this->_initPipes() ||
        !this->_forkCgi())
        return;

    if (_pid == 0)
        this->_runCgi();
    else
        this->lol();
}