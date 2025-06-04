
#pragma once 

#include <string>
#include "MyConfig.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"

class	ConnectionHandler {
	private:
		HTTPRequest _request;
		// HTTPResponse _httpRequest;
		// HTTPAction *_HTTPAction
		// MyConfig &_myConfig;
		// std::string requestBuffer;
		int _connectionSocketFD;

	public:
		ConnectionHandler();
		ConnectionHandler(int input);
		ConnectionHandler(const ConnectionHandler &other);
		ConnectionHandler &operator=(const ConnectionHandler &other);
		~ConnectionHandler();

		void makeResponse(std::string &input);
		std::string getResponse();

		// void setConnectionSocketFD(int input);
		int getConnectionSocketFD();
		// HTTPRequest receiveRequest();
		// void sendResponse(int socketFD, HTTPResponse finishedHTTPResponse);

		bool isAutoClose();
};


