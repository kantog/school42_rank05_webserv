/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kvanden- <kvanden-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 16:34:57 by kvanden-          #+#    #+#             */
/*   Updated: 2025/06/05 11:20:19 by kvanden-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/HTTPRequest.hpp"
#include <sstream>
#include <iostream>

HTTPRequest::HTTPRequest() {}

HTTPRequest::HTTPRequest(std::string method,
                         std::string requestTarget,
                         std::string hostURL,
                         const std::map<std::string, std::string> &headers,
                         std::string body)
{
    setMethod(method);
    setRequestTarget(requestTarget);
    setHostURL(hostURL);
    setHeaders(headers);
    setBody(body);
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

void HTTPRequest::setMethod(const std::string &input)
{
    _method = input;
}

void HTTPRequest::setRequestTarget(const std::string &input)
{
    _requestTarget = input;
}

void HTTPRequest::setHostURL(const std::string &input)
{
    _hostURL = input;
}

void HTTPRequest::setHeaders(const std::map<std::string, std::string> &input)
{
    _headers = input;
}

void HTTPRequest::setBody(const std::string &input)
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

bool HTTPRequest::isAutoClose() const
{
    /// @todo
    return true;
}

void HTTPRequest::fillHeaders(std::string line)
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
            setHostURL(value);
        }
        _headers[key] = value;
    }
}

void HTTPRequest::print() const
{
    std::cout << "lol Method: " << _method << std::endl;
    std::cout << "Request Target: " << _requestTarget << std::endl;
    std::cout << "Host URL: " << _hostURL << std::endl;
    std::cout << "Headers: " << std::endl;
    for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
    {
        std::cout << it->first << ": " << it->second << std::endl;
    }
    std::cout << "Body: " << _body << std::endl;
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
        setMethod(method);
        setRequestTarget(target);
    }

    while (std::getline(stream, line))
    {
        if (line == "\r" || line.empty())
        {
            isBody = true;
            continue;
        }

        if (!isBody)
            this->fillHeaders(line);
        else
            body += line + "\n";
    }
    setBody(body);

    #ifdef DEBUG
        this->print();
    #endif
}
