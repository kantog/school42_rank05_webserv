#pragma once

#include "ServerConfig.hpp"

#include <string>
#include <vector>
#include <map>

// #include <iostream>
// #include <fstream>
// #include <sstream>
// #include <algorithm>

class MyConfig
{
	private:
		static MyConfig *_myConfig;
		MyConfig();
		MyConfig(const char *filename);


	public:
		~MyConfig();
		static MyConfig const &get(const char *filename = NULL);
		std::map<std::string, std::vector<ServerConfig> > _servers;

		static ServerConfig const *getServerConfig(const std::string &serverKey, const std::string &hostURL);
		const ServerConfig *findServerConfig(const std::string &serverKey, const std::string &hostURL) const; // TODO
};
