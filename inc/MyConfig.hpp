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
    std::string redirectPath;
    int redirectCode;
    std::string root;
    bool directoryListing;
    std::vector<std::string> defaultFiles;
    std::map<std::string, std::string> cgiExtensions;
    bool allowUpload;
    std::string uploadPath;

    Route() : directoryListing(false), allowUpload(false) {}
};

class ServerConfig
{
public:
    std::string host;
    std::string port;
    std::string root;
    std::vector<std::string> server_names;
    std::map<int, std::string> error_pages;
    size_t client_max_body_size;
    std::vector<Route> routes;

    std::string getServerKey(void) const;
};

class MyConfig
{
private:
    static MyConfig *_myConfig;
    MyConfig();
    MyConfig(const char *filename);


public:
    ~MyConfig();
    static const MyConfig &get(const char *filename = NULL);
    
    std::map<std::string, std::vector<ServerConfig> > _servers;
    

    const ServerConfig *findServerConfig(const std::string &serverKey, const std::string &hostURL) const;

    static const ServerConfig *getServerConfig(const std::string &serverKey, const std::string &hostURL);

};
