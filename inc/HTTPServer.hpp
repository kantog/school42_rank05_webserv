
#pragma once 

#include <string>
#include <vector>
#include "ConnectionHandler.hpp"
#include "MyConfig.hpp"

class	HTTPServer {
	private:
		int _socketFD;
		int _epollFD;
		std::vector <ConnectionHandler>_ConnectionHandlers;
		MyConfig &_myConfig;

	public:
		HTTPServer();
		HTTPServer(const HTTPServer &other);
		HTTPServer &operator=(const HTTPServer &other);
		~HTTPServer();

		void init();
		void start();
		void stop();
};


