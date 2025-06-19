#include "Path.hpp"

#include <algorithm>
#include <cstring>

Path::Path() : _path("")
{
}

Path::Path(const std::string &path) : _path(path)
{
    normalize();
}

Path::Path(const char *path) : _path(path ? path : "")
{
    normalize();
}

Path::Path(const Path &other) : _path(other._path)
{
}

void Path::normalize()
{
    if (_path.empty())
        return;

    // Replace multiple slashes with single slash
    std::string result;
    result.reserve(_path.length());
    bool lastWasSlash = false;

    for (size_t i = 0; i < _path.length(); ++i)
    {
        char c = _path[i];
        if (c == '/')
        {
            if (!lastWasSlash)
            {
                result += c;
                lastWasSlash = true;
            }
        }
        else
        {
            result += c;
            lastWasSlash = false;
        }
    }

    _path = result;
}

Path &Path::operator=(const std::string &path)
{
    _path = path;
    normalize();
    return *this;
}

Path &Path::operator=(const Path &other)
{
    if (this != &other)
    {
        _path = other._path;
    }
    return *this;
}

Path Path::operator+(const Path &other) const
{
    return join(other);
}

Path Path::operator+(const std::string &other) const
{
    return join(other);
}

Path &Path::operator+=(const Path &other)
{
    *this = join(other);
    return *this;
}

Path &Path::operator+=(const std::string &other)
{
    *this = join(other);
    return *this;
}

bool Path::operator==(const Path &other) const
{
    return _path == other._path;
}

bool Path::operator!=(const Path &other) const
{
    return _path != other._path;
}

Path Path::join(const Path &other) const
{
    return join(other._path);
}

Path Path::join(const std::string &other) const
{
    if (_path.empty())
        return Path(other);
    if (other.empty())
        return *this;

    std::string result = _path;

    bool pathEndsWithSlash = (!result.empty() && result[result.length() - 1] == '/');
    bool otherStartsWithSlash = (!other.empty() && other[0] == '/');

    if (pathEndsWithSlash && otherStartsWithSlash)
        result += other.substr(1);
    else if (!pathEndsWithSlash && !otherStartsWithSlash)
        result += "/" + other;
    else
        result += other;
    return Path(result);
}

Path Path::removePrefix(const Path &prefix) const
{
    return removePrefix(prefix._path);
}

Path Path::removePrefix(const std::string &prefix) const
{
    if (prefix.empty())
        return *this;
    if (!startsWith(prefix))
        return *this;

    if (prefix == "/")
        return *this;

    std::string result = _path.substr(prefix.length());

    if (!result.empty() && result[0] != '/')
        result = "/" + result;

    return Path(result);
}

bool Path::startsWith(const Path &prefix) const
{
    return startsWith(prefix._path);
}

bool Path::startsWith(const std::string &prefix) const
{
    if (prefix.empty())
        return true;
    if (_path.length() < prefix.length())
        return false;
    if (_path.substr(0, prefix.length()) != prefix)
        return false;
    if (_path.length() == prefix.length())
        return true;
    return _path[prefix.length()] == '/';
}

bool Path::isEmpty() const
{
    return _path.empty();
}

std::string Path::getFilename() const
{
    std::string pathWithoutQuery = _path;

    size_t queryPos = pathWithoutQuery.find('?');
    if (queryPos != std::string::npos)
        pathWithoutQuery = pathWithoutQuery.substr(0, queryPos);

    size_t slashPos = pathWithoutQuery.find_last_of('/');
    if (slashPos == std::string::npos)
        return pathWithoutQuery;
    return pathWithoutQuery.substr(slashPos + 1);
}

Path Path::getDirectory() const
{
    size_t slashPos = _path.find_last_of('/');
    if (slashPos == std::string::npos)
        return Path("");
    if (slashPos == 0)
        return Path("/");
    return Path(_path.substr(0, slashPos));
}

const std::string &Path::toString() const
{
    return _path;
}

const char *Path::c_str() const
{
    return _path.c_str();
}

bool Path::isAbsolute() const
{
    return !_path.empty() && _path[0] == '/';
}

bool Path::isRelative() const
{
    return !isAbsolute();
}

Path Path::makeRelative() const
{
    if (_path.empty())
        return Path("./");
    if (_path.length() >= 2 && _path.substr(0, 2) == "./")
        return *this;
    return Path("./" + _path);
}

Path Path::makeAbsolute() const
{
    if (_path.length() >= 2 && _path.substr(0, 2) == "./")
        return Path(_path.substr(2));
    return *this;
}
