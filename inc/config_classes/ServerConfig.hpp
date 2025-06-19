
# pragma once

#include "Path.hpp"

#include <string>
#include <vector>
#include <map>

struct Route
{
	std::string path;
	std::vector<std::string> allowedMethods;
	std::string redirectPath;//?
	int redirectCode;//?
	std::string root;//necessary, or is only path enough?
	bool isDirectoryListing;
	std::vector<std::string> defaultFiles;
	std::map<std::string, std::string> cgiExtensions;//CGI arguments zitten ook hiern, nog te parsen
	bool uploadAllowed;
	std::string uploadPath;
	size_t client_max_body_size;

	Route() : isDirectoryListing(false), uploadAllowed(false), client_max_body_size(-1) {}
};

class ServerConfig
{
	private:
		mutable const Route *_curentRoute;
		mutable std::string _curentRoutePath;
		const struct Route *_findRoute(const std::string &path) const;

	public:
		ServerConfig();

		std::string host;
		std::string port;
		std::string root;
		std::vector<std::string> server_names;
		std::map<int, std::string> error_pages;
		size_t client_max_body_size;
		std::vector<Route> routes;

		void setCorectRoute(const std::string &path) const;

		bool isAllowedCgi(const std::string &fullPath) const;
		bool isAllowedMethod(const std::string &method) const;
		bool isReturn(void) const;

		// const std::string &getCurentRoutePath(void) const { return _curentRoutePath; }
		const Route &getCurentRoute(void) const { return *_curentRoute; };

		std::string getFullFilesystemPath(const std::string &requestPath) const;
		// std::string getFullFilesystemPath(const Path &requestPath) const;
		// Path getFullFilesystemPath(const std::string &requestPath) const;
		// Path getFullFilesystemPath(const Path &requestPath) const;

		Path getDocumentRoot(void) const;


		std::string getServerKey(void) const;
		const std::string getCgiInterpreter(const std::string &fullPath) const;
		const std::string getErrorPagePath(int code) const;
		size_t getClientMaxBodySize(void) const;
};
