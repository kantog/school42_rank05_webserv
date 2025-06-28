#include "../../inc/config_classes/ServerConfig.hpp"
#include "Defines.hpp"

#include <sstream>

ServerConfig::ServerConfig():
    _curentRoute(NULL),
    client_max_body_size(0)
{}

const Route *ServerConfig::_findRoute(const std::string &path) const
{
    const Route* bestMatch = NULL;
    size_t bestMatchLength = 0;

    for (std::vector<Route>::const_iterator it = routes.begin(); it != routes.end(); ++it)
    {
        const std::string &routePath = it->path;

        if (path.compare(0, routePath.length(), routePath) == 0 &&
            (path.length() == routePath.length() || path[routePath.length()] == '/' || routePath == "/"))
        {
            if (routePath.length() > bestMatchLength)
            {
                bestMatch = &(*it);
                bestMatchLength = routePath.length();
            }
        }
    }

    if (!bestMatch)
        return &routes[0];

    return bestMatch;
}

void ServerConfig::setCorrectRoute(const std::string &path) const
{
    if (_curentRoutePath == path)
        return;
    _curentRoutePath = path;
    _curentRoute = _findRoute(path);
}

bool ServerConfig::isAllowedCgi(const std::string &fullPath) const
{
    // vb "./www/cgi/slow.py/extra.v1/info?foo=bar&name=jan"
    const std::string extension = fullPath.substr(fullPath.find_last_of(".") + 1);
    return (_curentRoute->cgiExtensions.find(extension) != _curentRoute->cgiExtensions.end()
        || _curentRoute->cgiExtensions.find("." + extension) != _curentRoute->cgiExtensions.end());
}

bool ServerConfig::isAllowedMethod(const std::string &method) const
{
    if (_curentRoute->allowedMethods.empty())
        return true;
    for (std::vector<std::string>::const_iterator it = _curentRoute->allowedMethods.begin(); it != _curentRoute->allowedMethods.end(); ++it)
    {
        if (*it == method)
            return true;
    }
    return false;
}

bool ServerConfig::isReturn(void) const
{
    return _curentRoute->redirectPath != "";
}

std::string ServerConfig::getServerKey(void) const
{
    std::stringstream ss;
    ss << host << ":" << port;
    return ss.str();
}

std::string ServerConfig::getFullFilesystemPath(const std::string &requestPath) const
{    
    Path documentRoot = getDocumentRoot();
    Path request(requestPath);
    Path routePath(_curentRoute->path);
    
    Path relativePath = request.removePrefix(routePath);
    Path fullPath = documentRoot.join(relativePath);    
    return fullPath.makeRelative().toString();
}

Path ServerConfig::getDocumentRoot(void) const
{
    if (!_curentRoute->root.empty()) {
        return Path(_curentRoute->root);
    }
    return Path(this->root);
}

std::string ServerConfig::getUploadPath(Path relativePath) const
{

    if (!_curentRoute->uploadAllowed)
        return "";
    Path rawPath = _curentRoute->uploadPath;
    Path rootedPath;
    if (rawPath.isRelative())
        rootedPath = rawPath;
    else
        rootedPath = getDocumentRoot().join(rawPath).makeRelative();

    Path fileName = relativePath.removePrefix(_curentRoute->path);
    if (fileName.isEmpty())
        return rootedPath.toString();
    return rootedPath.getDirectory().join(fileName).toString();
}

size_t ServerConfig::getClientMaxBodySize(void) const
{
    if (_curentRoute->client_max_body_size != 0)
        return _curentRoute->client_max_body_size;
    return this->client_max_body_size;
}

const std::string ServerConfig::getCgiInterpreter(const std::string &fullPath) const
{
    const std::string extension = fullPath.substr(fullPath.find_last_of(".") + 1);
    std::map<std::string, std::string>::const_iterator it;
    it = _curentRoute->cgiExtensions.find(extension);
    if (it != _curentRoute->cgiExtensions.end())
        return it->second;

    it = _curentRoute->cgiExtensions.find("." + extension);
    if (it != _curentRoute->cgiExtensions.end())
        return it->second;

    return "";
}

const std::string ServerConfig::getErrorPagePath(int code) const
{
    if (this->error_pages.find(code) != this->error_pages.end())
        return ("./" + this->root + this->error_pages.at(code));
    return ("");
}
