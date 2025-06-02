
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
		const MyConfig &_myConfig;

	public:
		HTTPServer(const MyConfig &_myConfig);
		HTTPServer(const HTTPServer &other);
		HTTPServer &operator=(const HTTPServer &other);
		~HTTPServer();

		void init();
		void start();
		void stop();
};


