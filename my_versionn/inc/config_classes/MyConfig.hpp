#pragma once

#include "ServerConfig.hpp"

#include <string>
#include <vector>
#include <map>

class MyConfig
{
	private:
		MyConfig();
		MyConfig(const char *filename);

	public:
		std::map<std::string, std::vector<ServerConfig> > _servers;

		~MyConfig();

		static MyConfig const &get(const char *filename = NULL);
		static ServerConfig const *getServerConfig(const std::string &serverKey, const std::string &hostURL);// get en find? wat is het verschil?
		const ServerConfig *findServerConfig(const std::string &serverKey, const std::string &hostURL) const;//
};
