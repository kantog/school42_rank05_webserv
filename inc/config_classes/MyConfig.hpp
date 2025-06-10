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
		static const MyConfig &get(const char *filename = NULL);
		std::map<std::string, std::vector<ServerConfig> > _servers;

		static const ServerConfig *getServerConfig(const std::string &serverKey, const std::string &hostURL);
		const ServerConfig *findServerConfig(const std::string &serverKey, const std::string &hostURL) const;
};
