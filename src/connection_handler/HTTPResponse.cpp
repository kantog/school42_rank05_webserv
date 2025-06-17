

#include "../../inc/connection_handler/HTTPResponse.hpp"

#include <cerrno>
#include <fstream>
#include <sstream>
#include <stdexcept>

HTTPResponse::HTTPResponse() : 
	_statusCode(0), 
	_statusText("")
{ }

HTTPResponse::~HTTPResponse() {}

void HTTPResponse::reset()
{
    _statusCode = 200;
    _statusText = "OK";
    _headers.clear();
    _body.clear();
    _responseString.clear();

    // setHeader("Content-Length", "0");
}

void HTTPResponse::setStatusCode(int code)
{
    _statusCode = code;
    _setStatusMessage(code);
}

void HTTPResponse::setStatusMessage(const std::string &message)
{
    _statusText = message;
}

void HTTPResponse::setHeader(const std::string &key, const std::string &value)
{
    _headers[key] = value;
}

void HTTPResponse::setBody(const std::string &body, const std::string &contentType)
{
    _body = body + "\n\r";

    setBodySize();
    setHeader("Content-Type", contentType);
}

void HTTPResponse::setBodySize()
{
    std::ostringstream oss;
    oss << _body.size();
    setHeader("Content-Length", oss.str());
}

void HTTPResponse::setBodyFromFile(const std::string &filePath, const std::string &contentType)
{
    std::ifstream file(filePath.c_str(), std::ios::in | std::ios::binary);
		
    if (!file.is_open())
    {
		int error = errno;
		if (file.bad())
			this->setStatusCode(500);
		else if (file.fail())
		{
			if (error == EACCES)	
				this->setStatusCode(403);
			if (error == ENOENT)	
				this->setStatusCode(404);
		}
		else 
			this->setStatusCode(400);
		return;
    }
    std::ostringstream buffer;
    buffer << file.rdbuf();
    file.close();

    setBody(buffer.str(), contentType);
}

void HTTPResponse::setRedirect(const std::string &location, int code)
{
    setStatusCode(code);
    setHeader("Location", location);
    setHeader("Content-Length", "0");
}

const int &HTTPResponse::getStatusCode() const
{
	return _statusCode;
}

const std::string &HTTPResponse::getResponseString() const
{
    return _responseString;
}

void HTTPResponse::_setStatusMessage(int code)
{
    /// @todo
    switch (code)
    {
    case 200:
        _statusText = "OK";
        break;
    case 404:
        _statusText = "Not Found";
        break;
    case 500:
        _statusText = "Internal Server Error";
        break;
    default:
        _statusText = "Unknown";
        break;
    }
}

void HTTPResponse::buildErrorPage(int code, const std::string &filePath)
{
    setStatusCode(code);
    setBodyFromFile(filePath);
    buildResponse();
}

void HTTPResponse::buildReturnPage(int code, const std::string &filePath)
{
    setStatusCode(code);
    setHeader("Location", filePath);
    setHeader("Content-Length", "0");
    buildResponse();
}

void HTTPResponse::buildResponse()
{
    std::ostringstream responsStream;

    responsStream << "HTTP/1.1 " << _statusCode << " " << _statusText << "\r\n";
    for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
    {
        responsStream << it->first << ": " << it->second << "\r\n";
    }
    responsStream << "\r\n";
    if (_body.size() > 0)
        responsStream << _body;

    _responseString = responsStream.str();
}
