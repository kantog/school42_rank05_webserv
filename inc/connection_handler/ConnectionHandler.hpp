
#pragma once 

#include <string>
#include "../config_classes/MyConfig.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "HTTP_actions/AHTTPAction.hpp"

class	ConnectionHandler 
{
	private:
		HTTPRequest _request; //nog handlen: onvolledige requests binnenkrijgen, check wat recv doet exact
		HTTPResponse _response;
		AHTTPAction *_AHTTPAction;
		// MyConfig &_myConfig;
		int _connectionSocketFD;

		ConnectionHandler();

		int _getConnectionSocketFD();

		// void makeResponse(std::string &input);
		void _createRequest();
		void _sendResponse();


	public:
		ConnectionHandler(int input);
		ConnectionHandler(const ConnectionHandler &other);
		ConnectionHandler &operator=(const ConnectionHandler &other);
		~ConnectionHandler();

		void handleHTTP();
		bool shouldClose();
};
