
#include "../../inc/config_classes/ServerConfig.hpp"
#include <sstream>

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

    // TODO: check
    if (!bestMatch)
        return &routes[0];

    return bestMatch;
}

void ServerConfig::setCorectRoute(const std::string &path) const
{
    if (_curentRoutePath == path)
        return;
    _curentRoutePath = path;
    _curentRoute = _findRoute(path);
}

bool ServerConfig::isAllowedCgi(const std::string &fullPath) const
{
    return _curentRoute->cgiExtensions.find(fullPath.substr(fullPath.find_last_of(".") + 1)) != _curentRoute->cgiExtensions.end();
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

std::string ServerConfig::getFullPath(const std::string &path, const std::string &file) const
{
    if (_curentRoute->root != "")
        return _curentRoute->root + "/" + path + "/" + file;
    return this->root + "/" + path + "/" + file;
}

const std::string &ServerConfig::getFullCgiPath(const std::string &fullPath) const
{
    // TODO can een exception worden gegooid als path niet bestaat
    return _curentRoute->cgiExtensions.at(fullPath.substr(fullPath.find_last_of(".") + 1));
}