
#include "../../inc/config_classes/ServerConfig.hpp"
#include <sstream>

ServerConfig::ServerConfig():
	root(""),
    client_max_body_size(-1)
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

const std::string ServerConfig::getPath(void) const
{
    if (_curentRoute->root != "")
        return "." + _curentRoute->root;
    return "." + this->root;
}

std::string ServerConfig::getFullPath(const std::string &path) const
{
    /* TODO
        Define a directory or file where the requested file should be located (e.g.,
        if url /kapouet is rooted to /tmp/www, url /kapouet/pouic/toto/pouet is
        /tmp/www/pouic/toto/pouet).
    */
    return getPath() + path;
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

    return ""; // TODO
}

const std::string ServerConfig::getErrorPagePath(int code) const
{
    if (this->error_pages.find(code) != this->error_pages.end())
        return ("." + this->root + this->error_pages.at(code));
    return ("./default/defaultError.html"); // TODO: define in haeder?
}
