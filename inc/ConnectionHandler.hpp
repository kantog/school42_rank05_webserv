
#pragma once 

#include <string>
#include "MyConfig.hpp"

class	ConnectionHandler {
	private:
		// HTTPRequest _httpRequest;
		// HTTPResponse _httpRequest;
		// HTTPAction *_HTTPAction
		MyConfig &_myConfig;
		std::string requestBuffer;

	public:
		ConnectionHandler();
		ConnectionHandler(const ConnectionHandler &other);
		ConnectionHandler &operator=(const ConnectionHandler &other);
		~ConnectionHandler();

		// HTTPRequest receiveRequest();
		// void sendResponse(int socketFD, HTTPResponse finishedHTTPResponse);
};


