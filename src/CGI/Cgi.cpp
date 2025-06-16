

#include "Cgi.hpp"

#include <sys/stat.h>


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

Cgi::Cgi(const HTTPRequest &request, const ServerConfig &serverConfig) :
_request(request),
_serverConfig(serverConfig)
{
    this->_parsePath();
    this->_checkAccess(); // TODO denk dat dit weg mag
}

void Cgi::_parsePath()
{
    // vb /cgi-bin/script.py/extra.v1/info?foo=bar&name=jan
	std::string path = _serverConfig.getFullPath(_request.getRequestTarget());

    size_t qmark = path.find('?');
    if (qmark != std::string::npos)
    {
        _query = path.substr(qmark + 1);
        path = path.substr(0, qmark);
    }

    for (int i = path.length(); i >= 0; --i)
    {
        if (path[i] == '/')
        {
            _path = path.substr(0, i);
            this->_checkAccess();
            if (_statusCode == 200)
            {
                _pathInfo = path.substr(i);
                return;
            }
        }
    }
}

void Cgi::_checkAccess()
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
}

void Cgi::run()
{

}