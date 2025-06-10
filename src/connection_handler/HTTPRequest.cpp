/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kvanden- <kvanden-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 16:34:57 by kvanden-          #+#    #+#             */
/*   Updated: 2025/06/10 19:59:27 by kvanden-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/connection_handler/HTTPRequest.hpp"
#include <sstream>
#include <iostream>

HTTPRequest::HTTPRequest() {
    _isComplete = false;
    _currentFunction = &HTTPRequest::_setMethod;
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
        this->_currentFunction = &HTTPRequest::_setBody;
        return;
    }
    this->_fillHeaders(line);
}

const std::string HTTPRequest::decodeChunkedBody(const std::string &chunkedBody)
{

    static size_t chunkSize = 0;
    static bool done = false;

    if (!chunkSize && !done)
    {
        std::stringstream hexStream(chunkedBody);
        hexStream >> std::hex >> chunkSize;
        if (chunkSize == 0)
            done = true; // TODO: check
        return "";
    }
    else if (done)
    {
        done = false;
        _isComplete = true;
        return "";
    }
    return chunkedBody;
}

void HTTPRequest::_setBody(std::string &line)
{

    if (this->getHeader("Content-Length") == "0")
    {
        this->_isComplete = true;
        return;
    }
    else if (this->getHeader("Transfer-Encoding") == "chunked")
    {
        _addLineToBody(decodeChunkedBody(line));
    }
    else
    {
        _addLineToBody(line);
        if (_contentLength == _body.length())
            this->_isComplete = true;
        // TODO : is er niet te veel in de body?
    }
    if (_contentLength > _body.length())
        this->_isComplete = true; // TODO: check
    
}

void HTTPRequest::_addLineToBody(std::string line)
{
    _body += line;
}

void HTTPRequest::parseRequest(const char *rawRequest)
{
    _requestBuffer.append(rawRequest);
    std::cout << "requestBuffer: " << _requestBuffer << std::endl;
    std::string processedBuffer;
    std::string line;
    size_t pos = 0;

    while ((pos = _requestBuffer.find('\n')) != std::string::npos && !_isComplete)
    {
        line = _requestBuffer.substr(0, pos);

        if (!line.empty() && line[line.length() - 1] == '\r')
        {
            line.erase(line.length() - 1);
        }

        (this->*_currentFunction)(line);
        _requestBuffer.erase(0, pos + 1);
    }

#ifdef DEBUG
    std::cout << "\n";
    this->_printRequest();
#endif
}
