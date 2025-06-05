
#pragma once 

#include <string>
#include "MyConfig.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"

class	ConnectionHandler {
	private:
		HTTPRequest _request; //nog handlen: onvolledige requests binnenkrijgen, check wat recv doet exact
		// HTTPResponse _httpRequest; // toevoegen: reset()
		// HTTPAction *_HTTPAction
		// MyConfig &_myConfig;
		// std::string requestBuffer;
		int _connectionSocketFD;

		void makeResponse(std::string &input);
		std::string getResponse();

		// void setConnectionSocketFD(int input);
		int getConnectionSocketFD();
		void receiveRequest();
		// void sendResponse(int socketFD, HTTPResponse finishedHTTPResponse);
		//
	public:
		ConnectionHandler();
		ConnectionHandler(int input);
		ConnectionHandler(const ConnectionHandler &other);
		ConnectionHandler &operator=(const ConnectionHandler &other);
		~ConnectionHandler();

		void handleHTTP();
		bool isAutoClose();
};


