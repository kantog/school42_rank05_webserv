
#pragma once 

#include <string>
#include <vector>
#include "ConnectionHandler.hpp"
// #include "MyConfig.hpp"

class	HTTPServer {
	private:
		int _listeningSocketFD;
		int _epollFD;
		int _connAmount;
		std::vector <ConnectionHandler> _connectionHandlers;//test uncomment
		// const MyConfig &_myConfig;//test uncomment
		static const int _maxEpollEvents = 32;

		void initSocket();
		void initEpoll();

	public:
		// HTTPServer(const MyConfig &_myConfig);
		HTTPServer();//test
		HTTPServer(const HTTPServer &other);
		HTTPServer &operator=(const HTTPServer &other);
		~HTTPServer();

		void init();
		void start();
		void stop();
};
