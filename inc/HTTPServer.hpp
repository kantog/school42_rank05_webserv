
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
		std::vector <ConnectionHandler> _connectionHandlers;// turn into map? //test uncomment
		// const MyConfig &_myConfig;//test uncomment
		static const int _maxEpollEvents = 32;

		void initListeningSocket();
		void initEpoll();
		void createNewConnection();

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
