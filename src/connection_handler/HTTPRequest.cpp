/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kvanden- <kvanden-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 16:34:57 by kvanden-          #+#    #+#             */
/*   Updated: 2025/06/11 18:26:38 by kvanden-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/connection_handler/HTTPRequest.hpp"
#include <sstream>
#include <iostream>

HTTPRequest::HTTPRequest()
{
    _isComplete = false;
    _currentFunction = &HTTPRequest::_setMethod;
    _chunkSizeRemaining = 0;
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
    return this->getHeader("Host"); // TODO
}

const std::map<std::string, std::string> &HTTPRequest::getHeaders() const
{
    return _headers;
}

const std::string &HTTPRequest::getBody() const
{
    return _body;
}

const std::string &HTTPRequest::getHeader(const std::string &key) const
{
    static const std::string empty = "";
    std::map<std::string, std::string>::const_iterator it = _headers.find(key);
    return it != _headers.end() ? it->second : empty;
}

bool HTTPRequest::hasCloseHeader() const // TODO: different name
{
    // TODO: check
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

void HTTPRequest::_printRequest() const
{
    std::cout << "\tMethod: " << _method << std::endl;
    std::cout << "\tRequest Target: " << _requestTarget << std::endl;
    std::cout << "\tHost URL: " << getHeader("Host") << std::endl;
    // std::cout << "\tHeaders: " << std::endl;
    // for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
    //     std::cout << it->first << ": " << it->second << std::endl;
    std::cout << "\tBody: " << _body.substr(0, 100) << "..." << std::endl;
}

bool HTTPRequest::isComplete() const
{
    return _isComplete;
}

void HTTPRequest::_setMethod(std::string &line)
{
    std::istringstream startLine(line);

    startLine >> _method >> _requestTarget >> _version;
    // TODO: version check error?
    this->_currentFunction = &HTTPRequest::_setHeader;
}

void HTTPRequest::_setHeader(std::string &line)
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
            this->_isComplete = true; // TODO: check
        this->_currentFunction = NULL;
        return;
    }
    this->_fillHeaders(line);
}

bool HTTPRequest::_setchunkSize()
{
    size_t pos = _requestBuffer.find('\n');
    if (pos == std::string::npos)
        return (false); // Need more data

    std::string sizeLine = _requestBuffer.substr(0, pos);
    if (!sizeLine.empty() && sizeLine[sizeLine.length() - 1] == '\r')
        sizeLine.erase(sizeLine.length() - 1);

    std::stringstream hexStream(sizeLine);
    hexStream >> std::hex >> _chunkSizeRemaining;

    _requestBuffer.erase(0, pos + 1);

    if (_chunkSizeRemaining == 0)
    {
        _isComplete = true;
        return (false);
    }
    return (true);
}

void HTTPRequest::_trimChunked()
{
    if (_chunkSizeRemaining)
        return;
    if (_requestBuffer.length() >= 2 &&
        _requestBuffer.substr(0, 2) == "\r\n")
        _requestBuffer.erase(0, 2);
    else if (_requestBuffer.length() >= 1 &&
             _requestBuffer[0] == '\n')
        _requestBuffer.erase(0, 1);
}

bool HTTPRequest::_addChunkData()
{
    size_t availableBytes = _requestBuffer.length();
    size_t bytesToCopy = std::min(_chunkSizeRemaining, availableBytes);

    if (bytesToCopy > 0)
    {
        _body.append(_requestBuffer.substr(0, bytesToCopy));
        _requestBuffer.erase(0, bytesToCopy);
        _chunkSizeRemaining -= bytesToCopy;

        _trimChunked();
        return (true);
    }
    return (false); // Need more data
}

void HTTPRequest::_parseChunkedBody()
{
    while (!_requestBuffer.empty() && !_isComplete)
    {
        if (_chunkSizeRemaining == 0)
        {
            if (!_setchunkSize())
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

    size_t neededBytes = _contentLength - _body.length();
    size_t availableBytes = _requestBuffer.length();
    size_t bytesToCopy = std::min(neededBytes, availableBytes);

    if (bytesToCopy > 0)
    {
        _body.append(_requestBuffer.substr(0, bytesToCopy));
        _requestBuffer.erase(0, bytesToCopy);
    }

    if (_body.length() >= _contentLength)
        _isComplete = true;
}

void HTTPRequest::parseRequest(const char *rawRequest)
{
    _requestBuffer.append(rawRequest);
    std::cout << "requestBuffer: " << _requestBuffer << std::endl;

    if (_currentFunction == NULL)
    {
        _setBody();
        return;
    }

    std::string line;
    size_t pos = 0;

    while ((pos = _requestBuffer.find('\n')) != std::string::npos)
    {
        line = _requestBuffer.substr(0, pos);

        if (!line.empty() && line[line.length() - 1] == '\r')
            line.erase(line.length() - 1);

        (this->*_currentFunction)(line);
        _requestBuffer.erase(0, pos + 1);

        if (_currentFunction == NULL)
            break;
    }

    // #ifdef DEBUG
    //     std::cout << "\n";
    //     this->_printRequest();
    // #endif
}
