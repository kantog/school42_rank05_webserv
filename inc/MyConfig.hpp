#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

struct Route
{
    const std::string path;
    const std::vector<std::string> allowedMethods;
    const std::string redirect;
    const std::string root;
    const bool directoryListing;
    const std::vector<std::string> defaultFiles;
    const std::map<std::string, std::string> cgiExtensions;
    const bool allowUpload;
    const std::string uploadPath;

    Route() : directoryListing(false), allowUpload(false) {}
};

struct ServerConfig
{
    const std::vector<std::string> hosts;
    const int port;
    const std::vector<std::string> server_names;
    const std::map<int, std::string> error_pages;
    const size_t client_max_body_size;
    const std::vector<Route> routes;
};


class MyConfig
{
private:
    MyConfig();
    MyConfig(const char* filename);

public:

    std::vector<ServerConfig> _servers;//work in progress
    static MyConfig& get(const char* filename = NULL);
    ~MyConfig();    
};
