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
    std::string path;
    std::vector<std::string> allowedMethods;
    std::string redirect;
    std::string root;
    bool directoryListing;
    std::vector<std::string> defaultFiles;
    std::map<std::string, std::string> cgiExtensions;
    bool allowUpload;
    std::string uploadPath;

    Route() : directoryListing(false), allowUpload(false) {}
};

struct ServerConfig
{
    std::vector<std::string> hosts;
    int port;
    std::vector<std::string> server_names;
    std::map<int, std::string> error_pages;
    size_t client_max_body_size;
    std::vector<Route> routes;
};


class MyConfig
{
private:

    MyConfig(const char* filename);
    std::vector<ServerConfig> _servers;
public:

    static MyConfig& get(const char* filename = NULL);
    static MyConfig& get() { return get(NULL); }
    ~MyConfig();    
};