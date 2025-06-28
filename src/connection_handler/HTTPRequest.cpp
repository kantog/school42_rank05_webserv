/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kvanden- <kvanden-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 16:34:57 by kvanden-          #+#    #+#             */
/*   Updated: 2025/06/16 15:15:07 by kvanden-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/connection_handler/HTTPRequest.hpp"
#include "ServerConfig.hpp"
#include "MyConfig.hpp"
#include "Defines.hpp"

#include <sstream>
#include <iostream>
#include <unistd.h>

HTTPRequest::HTTPRequest()
{
    _isComplete = false;
    _currentFunction = &HTTPRequest::_setMethod;
    _chunkSizeRemaining = 0;
    _bodyBytesReceived = 0;
    _errorCode = 200;
}

HTTPRequest::HTTPRequest(const HTTPRequest &other)
{
    *this = other;
}

HTTPRequest &HTTPRequest::operator=(const HTTPRequest &other)
{
    if (this != &other)
    {
        _method = other._method;
        _requestTarget = other._requestTarget;
        _headers = other._headers;
        _body = other._body;
    }
    return *this;
}

HTTPRequest::~HTTPRequest() {}

void HTTPRequest::reset()
{
    _method.clear();
    _requestTarget.clear();
    _headers.clear();
    _body.clear();
    _isComplete = false;
    _currentFunction = &HTTPRequest::_setMethod;
    _chunkSizeRemaining = 0;
    _bodyBytesReceived = 0;
    _errorCode = 200;
}

const std::string &HTTPRequest::getRawPath() const
{
    return _rawPath;
}

const std::string &HTTPRequest::getMethod() const
{
    return _method;
}

const std::string &HTTPRequest::getRequestTarget() const
{
    return _requestTarget;
}

const std::string &HTTPRequest::getHostURL() const
{
    return this->getHeader("Host");
}

const std::map<std::string, std::string> &HTTPRequest::getHeaders() const
{
    return _headers;
}

const std::string &HTTPRequest::getBody() const
{
    return _body;
}

const std::string &HTTPRequest::getPathInfo() const
{
    return _pathInfo;
}

const std::string &HTTPRequest::getQuery() const
{
    return _query;
}

const std::string &HTTPRequest::getVersion() const
{
    return _version;
}

const std::string &HTTPRequest::getRequestFile() const
{
    return _requestFile;
}

const std::string &HTTPRequest::getHeader(const std::string &key) const
{
    static const std::string empty = "";
    std::map<std::string, std::string>::const_iterator it = _headers.find(key);
    return it != _headers.end() ? it->second : empty;
}

bool HTTPRequest::hasCloseHeader() const
{
    if (this->getHeader("Connection") == "close")
        return true;
    return false;
}

void HTTPRequest::_fillHeaders(std::string line)
{
    size_t colonPos = line.find(':');
    if (colonPos != std::string::npos)
    {
        std::string key = line.substr(0, colonPos);
        std::string value = line.substr(colonPos + 1);
        while (!value.empty() && (value[0] == ' ' || value[0] == '\t'))
            value.erase(0, 1);
        _headers[key] = value;
    }
}

void HTTPRequest::printRequest() const
{
    std::cout << "\tMethod: " << _method << std::endl;
    std::cout << "\tRequest Target: " << _requestTarget << std::endl;
    std::cout << "\tHost URL: " << getHeader("Host") << std::endl;
    std::cout << "\tHeaders: " << std::endl;
    for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
        std::cout << it->first << ": " << it->second << std::endl;
    std::cout << "\tBody: " << _body << "..." << std::endl;
}

bool HTTPRequest::isComplete() const
{
    return _isComplete;
}

void HTTPRequest::_parsePath(const std::string &serverKey)
{
    // vb /cgi-bin/script.py/extra.v1/info?foo=bar&name=jan
    ServerConfig const *serverConfig = MyConfig::getServerConfig(serverKey, this->getHostURL());
    serverConfig->setCorrectRoute(this->_rawPath);
    Path prefix = serverConfig->getDocumentRoot();
    std::string striptPath = _rawPath;

    size_t queryPos = _rawPath.find('?');
    if (queryPos != std::string::npos)
    {
        _query = _rawPath.substr(queryPos + 1);
        striptPath = _rawPath.substr(0, queryPos);
    }

    _pathInfo = "/";
    if (access((prefix + striptPath).makeRelative().c_str(), F_OK) == 0)
        _requestTarget = striptPath;
    else
    {
        for (int i = striptPath.length(); i >= 0; --i)
        {
            if (striptPath[i] == '/')
            {
                _requestTarget = striptPath.substr(0, i);
                if (access((prefix + striptPath).makeRelative().c_str(), F_OK) == 0)
                {
                    _pathInfo = striptPath.substr(i);
                    break;
                }
            }
        }
    }
    if (_requestTarget.empty())
        _requestTarget = striptPath;
    _requestFile = _requestTarget.substr(_requestTarget.find_last_of("/") + 1);
}

void HTTPRequest::_setMaxBodySize(const std::string &serverKey)
{
    ServerConfig const *_serverConfig = MyConfig::getServerConfig(serverKey, this->getHostURL());
    _serverConfig->setCorrectRoute(this->_requestTarget);
    _maxContentLength = _serverConfig->getClientMaxBodySize();
}

void HTTPRequest::_setMethod(std::string &line, const std::string &serverKey)
{
    (void)serverKey;
    std::istringstream startLine(line);

    startLine >> _method >> _rawPath >> _version;

    if (_version != "HTTP/1.1")
    {
        _errorCode = HTTP_VERSION_NOTSUPPORTED;
        return;
    }
    this->_currentFunction = &HTTPRequest::_setHeader;
}

void HTTPRequest::_setHeader(std::string &line, const std::string &serverKey)
{
    if (line == "\r" || line.empty())
    {
        _contentLength = 0;
        if (!this->getHeader("Content-Length").empty())
        {
            std::istringstream is(this->getHeader("Content-Length"));
            is >> _contentLength;
        }
        if (_contentLength == 0)
            this->_isComplete = true;

        this->_parsePath(serverKey);
        this->_setMaxBodySize(serverKey);

        if (_contentLength > _maxContentLength)
        {
            this->_isComplete = false;
            this->_errorCode = 413;
            return;
        }
        this->_currentFunction = NULL;
        return;
    }
    this->_fillHeaders(line);
}

bool HTTPRequest::_setChunkSize()
{
    std::vector<char>::iterator it = std::find(_requestBuffer.begin(), _requestBuffer.end(), '\n');
    if (it == _requestBuffer.end())
        return false; // Need more data

    std::string sizeLine(_requestBuffer.begin(), it); // van begin tot '\n'
    if (!sizeLine.empty() && sizeLine[sizeLine.size() - 1] == '\r')
        sizeLine.erase(sizeLine.size() - 1);

    std::stringstream hexStream(sizeLine);
    hexStream >> std::hex >> _chunkSizeRemaining;

    // Erase line + \n
    _requestBuffer.erase(_requestBuffer.begin(), it + 1);

    if (_chunkSizeRemaining == 0)
    {
        _isComplete = true;
        return false;
    }
    return true;
}

void HTTPRequest::_trimChunked()
{
    if (_chunkSizeRemaining > 0)
        return;

    if (_requestBuffer.size() >= 2 &&
        _requestBuffer[0] == '\r' &&
        _requestBuffer[1] == '\n')
    {
        _requestBuffer.erase(_requestBuffer.begin(), _requestBuffer.begin() + 2);
    }
    else if (_requestBuffer.size() >= 1 &&
             _requestBuffer[0] == '\n')
    {
        _requestBuffer.erase(_requestBuffer.begin());
    }
}

bool HTTPRequest::_addChunkData()
{
    size_t availableBytes = _requestBuffer.size();
    size_t bytesToCopy = (std::min)(_chunkSizeRemaining, availableBytes);

    if (bytesToCopy > 0)
    {
        _body.append(&_requestBuffer[0], bytesToCopy);
        _requestBuffer.erase(_requestBuffer.begin(), _requestBuffer.begin() + bytesToCopy);
        _chunkSizeRemaining -= bytesToCopy;

        _trimChunked();

        if (_body.size() >= _maxContentLength)
        {
            _errorCode = 413;
            _isComplete = false;
            return false;
        }
        return true;
    }
    return false; // Need more data
}


bool HTTPRequest::isError() const
{
    return _errorCode != 200;
}

void HTTPRequest::_parseChunkedBody()
{
    while (!_requestBuffer.empty() && !_isComplete)
    {
        if (_chunkSizeRemaining == 0)
        {
            if (!_setChunkSize())
                break;
        }
        else
        {
            if (!_addChunkData())
                break;
        }
    }
}

void HTTPRequest::_setBody()
{
    if (this->getHeader("Transfer-Encoding") == "chunked")
    {
        _parseChunkedBody();
        return;
    }

    if (!_requestBuffer.empty())
    {
        if (_body.capacity() < _contentLength)
            _body.reserve(_contentLength);

        _body.append(&_requestBuffer[0], _requestBuffer.size());
        _bodyBytesReceived += _requestBuffer.size();
        _requestBuffer.clear();
    }

    if (_bodyBytesReceived >= _contentLength)
    {
        if (_body.size() > _contentLength)
            _body.resize(_contentLength);
        _isComplete = true;
    }
}


void HTTPRequest::parseRequest(const char *rawRequest, ssize_t bytesRead, const std::string &serverKey)
{
    _requestBuffer.insert(_requestBuffer.end(), rawRequest, rawRequest + bytesRead);

    if (_currentFunction == NULL)
    {
        _setBody();
        return;
    }

    std::string line;

    while (true)
    {
        std::vector<char>::iterator it = std::find(_requestBuffer.begin(), _requestBuffer.end(), '\n');
        if (it == _requestBuffer.end())
            break;

        size_t lineLen = it - _requestBuffer.begin();
        line.assign(_requestBuffer.begin(), _requestBuffer.begin() + lineLen);

        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);

        (this->*_currentFunction)(line, serverKey);
        _requestBuffer.erase(_requestBuffer.begin(), it + 1);
        if (_errorCode != 200)
            return;
        if (_currentFunction == NULL)
            break;
    }

    if (_currentFunction == NULL)
        _setBody();
}
