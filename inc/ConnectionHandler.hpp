
#pragma once 

#include <string>
#include "MyConfig.hpp"

class	ConnectionHandler {
	private:
		// HTTPRequest _httpRequest;
		// HTTPResponse _httpRequest;
		// HTTPAction *_HTTPAction
		// MyConfig &_myConfig;
		// std::string requestBuffer;
		int _connectionSocketFD;

	public:
		ConnectionHandler(int input);
		ConnectionHandler(const ConnectionHandler &other);
		ConnectionHandler &operator=(const ConnectionHandler &other);
		~ConnectionHandler();

		// void setConnectionSocketFD(int input);
		int getConnectionSocketFD();
		// HTTPRequest receiveRequest();
		// void sendResponse(int socketFD, HTTPResponse finishedHTTPResponse);
};


