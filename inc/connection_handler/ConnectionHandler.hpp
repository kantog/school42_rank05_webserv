
#pragma once 

#include <string>
#include "../config_classes/MyConfig.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "HTTP_actions/HTTPAction.hpp"

class	ConnectionHandler 
{
	private:
		HTTPRequest _request; //nog handlen: onvolledige requests binnenkrijgen, check wat recv doet exact
		HTTPResponse _response;
		HTTPAction *_HTTPAction;
		// MyConfig &_myConfig;

		std::string &_serverKey;
		int _connectionSocketFD;
		const ServerConfig *_serverConfig;

		// ConnectionHandler();
		ConnectionHandler(const ConnectionHandler &other);
		ConnectionHandler &operator=(const ConnectionHandler &other);

		int _getConnectionSocketFD();

		// void makeResponse(std::string &input);
		void _createRequest();
		void _sendResponse();
		void _setServerConfig();

	public:
		ConnectionHandler(std::string &serverKey, int fd);
		~ConnectionHandler();

		void handleHTTP();
		bool shouldClose();
};
