
#pragma once 

#include <string>
#include "MyConfig.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"

class	ConnectionHandler 
{
	private:
		HTTPRequest _request; //nog handlen: onvolledige requests binnenkrijgen, check wat recv doet exact
		HTTPResponse _response;
		// HTTPAction *_HTTPAction;
		// MyConfig &_myConfig;

		std::string &_serverKey;
		int _connectionSocketFD;
		const ServerConfig *_serverConfig;

		// ConnectionHandler();
		ConnectionHandler(const ConnectionHandler &other);
		ConnectionHandler &operator=(const ConnectionHandler &other);

		int getConnectionSocketFD();

		// void makeResponse(std::string &input);
		void createRequest();
		void sendResponse();
		void setServerConfig();

	public:
		ConnectionHandler(std::string &serverKey, int fd);
		~ConnectionHandler();

		void handleHTTP();
		bool shouldClose();
};


