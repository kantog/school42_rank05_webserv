

#include "../../inc/connection_handler/HTTPResponse.hpp"
#include "ServerConfig.hpp"

#include <cerrno>
#include <cstddef>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <stdexcept>
#include <iostream>

HTTPResponse::HTTPResponse() : _statusCode(0),
                               _statusText("")
{
}

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

void HTTPResponse::setHeaders(const std::string &headers)
{
    std::istringstream headersStream(headers);
    std::string line;

    while (std::getline(headersStream, line))
    {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);

        if (line.empty())
            return;

        size_t colon = line.find(':');
        if (colon != std::string::npos)
        {
            std::string key = line.substr(0, colon);
            std::string value = line.substr(colon + 1);
            while (!value.empty() && (value[0] == ' ' || value[0] == '\t'))
                value.erase(0, 1);

            if (key == "Status")
            {
                // vb "Status: 200 OK"
                int statusCode;
                std::string statusMessage;
                std::istringstream statusStream(value);

                statusStream >> statusCode >> statusMessage;
                this->setStatusCode(statusCode);
                this->setStatusMessage(statusMessage);
            }
            else
                this->setHeader(key, value);
        }
    }
}

const std::string HTTPResponse::getContentTypeFromFile(const std::string &filePath)
{
    std::string contentType = "text/plain";

    size_t dot = filePath.find_last_of(".");
    if (dot != std::string::npos)
    {
        std::string ext = filePath.substr(dot + 1);

        if (ext == "html" || ext == "htm")
            contentType = "text/html";
        else if (ext == "css")
            contentType = "text/css";
        else if (ext == "js")
            contentType = "application/javascript";
        else if (ext == "png")
            contentType = "image/png";
        else if (ext == "jpg" || ext == "jpeg")
            contentType = "image/jpeg";
        else if (ext == "gif")
            contentType = "image/gif";
        else if (ext == "svg")
            contentType = "image/svg+xml";
        else if (ext == "ico")
            contentType = "image/x-icon";
        else if (ext == "json")
            contentType = "application/json";
        else if (ext == "txt")
            contentType = "text/plain";
        else
            contentType = "application/octet-stream";
    }
    return contentType;
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

void HTTPResponse::_setCustomErrorBody(const std::string &filePath)
{
    std::ostringstream oss;

    oss << "<!DOCTYPE html>\n"
        << "<html>\n"
        << "<head>\n"
        << "<title>Error " << this->_statusCode << "</title>\n"
        << "</head>\n"
        << "<body>\n"
        << "<h1>Error " << this->_statusCode << "</h1>\n"
        << "<p>" << _statusText << "</p>\n"
        << "<p>" << filePath << " not found</p>\n"
        << "</body>\n"
        << "</html>\n";

    this->setBody(oss.str());
}

void HTTPResponse::setBodyFromFile(const std::string &filePath)
{
    std::ifstream file(filePath.c_str(), std::ios::in | std::ios::binary);

    if (!file.is_open())
    {
        if (_statusCode != 200)
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
                // TODO: error kan ook 20 zijn
            }
            else
                this->setStatusCode(400);
        }
        this->_setCustomErrorBody(filePath);
        return;
    }
    std::ostringstream buffer;
    buffer << file.rdbuf();
    file.close();

    setBody(buffer.str(), getContentTypeFromFile(filePath));
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
    default: // kobe
        {
        std::cerr << "Unknown status code: " << code << std::endl;
        _statusText = "Unknown";
        break;
    }
    }
}

void HTTPResponse::buildErrorPage(int code, const std::string &filePath)
{
    setStatusCode(code);
    setBodyFromFile(filePath);
    buildResponse();
}

std::string HTTPResponse::_createDirString(const std::string &directoryPath,
                                           const std::string &appendString)
{
    std::string bodyToSet = "<br>";

    std::cout << "directory requested: " << directoryPath << std::endl; // test
    DIR *directory = opendir(directoryPath.c_str());
    if (!directory)
        throw std::runtime_error("Error: couldn't open directory");

    struct dirent *directoryInfo = readdir(directory);
    while (directoryInfo)
    {
        if (directoryInfo->d_type == DT_DIR && (static_cast<std::string>(directoryInfo->d_name)
                                                    .find_first_of(".") == static_cast<size_t>(-1)))
        {
            bodyToSet.append(directoryInfo->d_name);
            bodyToSet.append("/");
            bodyToSet.append(_createDirString(directoryPath + "/" + directoryInfo->d_name, "&nbsp &nbsp"));
        }
        else if (static_cast<std::string>(directoryInfo->d_name) != "." && static_cast<std::string>(directoryInfo->d_name) != "..")
        {
            bodyToSet.append(appendString);
            bodyToSet.append("<a href=\"");
            bodyToSet.append(directoryInfo->d_name);
            bodyToSet.append("\">");
            bodyToSet.append(directoryInfo->d_name);
            bodyToSet.append("</a> <br>");
        }
        directoryInfo = readdir(directory);
    }

    int errorCode = closedir(directory);
    if (errorCode == -1)
        throw std::runtime_error("Error while closing directory");
    return (bodyToSet);
}

void HTTPResponse::buildDirectoryPage(const std::string &directoryPath)
{
    this->setBody("INDEX\n" + this->_createDirString(directoryPath));
    if (_body.empty())                           // test
        std::cout << "empty body!" << std::endl; // test
}

void HTTPResponse::buildReturnPage(int code, const std::string &filePath) // TODO: test
{
    setStatusCode(code);
    setHeader("Location", filePath);
    setHeader("Content-Length", "0");
    buildResponse();
}

void HTTPResponse::buildCgiPage(const std::string &cgiString)
{
    std::string headers;
    std::string body;
    size_t header_end = cgiString.find("\r\n\r\n");
    if (header_end == std::string::npos)
        header_end = cgiString.find("\n\n");

    if (header_end != std::string::npos)
    {
        headers = cgiString.substr(0, header_end);
        body = cgiString.substr(header_end + (cgiString[header_end] == '\r' ? 4 : 2));
    }
    else
        body = cgiString;

    setStatusCode(200);
    setHeaders(headers);
    setBody(body);
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
