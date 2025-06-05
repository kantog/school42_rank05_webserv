
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
		int _connectionSocketFD;

		ConnectionHandler();

		int getConnectionSocketFD();

		// void makeResponse(std::string &input);
		void createRequest();
		void sendResponse();


	public:
		ConnectionHandler(int input);
		ConnectionHandler(const ConnectionHandler &other);
		ConnectionHandler &operator=(const ConnectionHandler &other);
		~ConnectionHandler();

		void handleHTTP();
		bool shouldClose();
};


