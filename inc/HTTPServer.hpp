
#pragma once 

#include <string>
#include "HTTPHandler.hpp"

class	HTTPServer {
	private:
		int _socketFD;
		int _epollFD;
		HTTPHandler &_HTTPHandler;
		std::string _ddd;

	public:
		HTTPServer();
		HTTPServer(int socketFD,
			int epollFD,
			std::string ccc,
			std::string ddd);
		HTTPServer(const HTTPServer &other);
		HTTPServer &operator=(const HTTPServer &other);
		~HTTPServer();
};


