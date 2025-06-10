/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kvanden- <kvanden-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 16:34:57 by kvanden-          #+#    #+#             */
/*   Updated: 2025/06/10 14:30:48 by kvanden-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/connection_handler/HTTPRequest.hpp"
#include <sstream>
#include <iostream>

HTTPRequest::HTTPRequest() {}

HTTPRequest::HTTPRequest(std::string method,
                         std::string requestTarget,
                         std::string hostURL,
                         const std::map<std::string, std::string> &headers,
                         std::string body)
{
    _setMethod(method);
    _setRequestTarget(requestTarget);
    _setHostURL(hostURL);
    _setHeaders(headers);
    _setBody(body);
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
        _hostURL = other._hostURL;
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
    _hostURL.clear();
    _headers.clear();
    _body.clear();
}

void HTTPRequest::_setMethod(const std::string &input)
{
    _method = input;
}

void HTTPRequest::_setRequestTarget(const std::string &input)
{
    _requestTarget = input;
}

void HTTPRequest::_setHostURL(const std::string &input)
{
    _hostURL = input;
}

void HTTPRequest::_setHeaders(const std::map<std::string, std::string> &input)
{
    _headers = input;
}

void HTTPRequest::_setBody(const std::string &input)
{
    _body = input;
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
    return _hostURL;
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

bool HTTPRequest::hasCloseHeader() const //TODO: different name
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
        if (key == "Host")
        {
            _setHostURL(value);
        }
        _headers[key] = value;
    }
}

void HTTPRequest::_printRequest() const
{
    std::cout << "\tMethod: " << _method << std::endl;
    std::cout << "\tRequest Target: " << _requestTarget << std::endl;
    std::cout << "\tHost URL: " << _hostURL << std::endl;
    // std::cout << "\tHeaders: " << std::endl;
    // for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
    //     std::cout << it->first << ": " << it->second << std::endl;
    std::cout << "\tBody: " << _body.substr(0, 100) << "..." << std::endl;
}

void HTTPRequest::parseRequest(std::string rawRequest)
{
    std::istringstream stream(rawRequest);
    std::string line;
    bool isBody = false;
    std::string body;

    if (std::getline(stream, line))
    {
        std::istringstream startLine(line);
        std::string method, target, version;
        startLine >> method >> target >> version;
        _setMethod(method);
        _setRequestTarget(target);
    }

    while (std::getline(stream, line))
    {
        if (line == "\r" || line.empty())
        {
            isBody = true;
            continue;
        }

        if (!isBody)
            this->_fillHeaders(line);
        else
            body += line + "\n";
    }
    _setBody(body);

    #ifdef DEBUG
		std::cout << "\n";
        this->_printRequest();
    #endif
}
