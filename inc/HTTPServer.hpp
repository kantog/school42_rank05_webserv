
#pragma once 

#include <string>
#include <vector>
#include "connection_handler/ConnectionHandler.hpp"

class	HTTPServer {
	private:
		int _listeningSocketFD;
		int _epollFD;
		int _connAmount;
		std::map<int, ConnectionHandler *> _connectionHandlers;
		static const int _maxEpollEvents = 32;

		void _initListeningSocket();
		void _initEpoll();
		void _createNewConnection();
		void _closeConnection(int connectionFd);
		void _delegateToConnectionHandler(int connectionFd);

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
