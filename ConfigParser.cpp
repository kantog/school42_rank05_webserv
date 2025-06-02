
#include "ConfigParser.hpp"

bool ConfigParser::openFile(const std::string& filename)
{
    std::ifstream file(filename);
    return file.is_open();
}

std::vector<ServerConfig> ConfigParser::parseConfigFile(const std::string& filename)
{
    if (!this->openFile(filename));
    {
        throw std::runtime_error("Failed to open file");
        return std::vector<ServerConfig>();
    }
    return std::vector<ServerConfig>();
}

void ConfigParser::printConfig(void)
{
    std::cout << "printConfig" << std::endl;
}
