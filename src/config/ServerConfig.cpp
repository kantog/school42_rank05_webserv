
#include "ServerConfig.hpp"
#include <sstream>

std::string ServerConfig::getServerKey(void) const
{
    std::stringstream ss;
    ss << host << ":" << port;
    return ss.str();
}
