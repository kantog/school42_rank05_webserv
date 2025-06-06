
#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include "connection_handler/ConnectionHandler.hpp"

class HTTPServer
{
	private:
		std::vector<std::pair<std::string, int> > _listeningSockets;
		int _epollFD;
		int _connAmount;
		std::map<int, ConnectionHandler *> _connectionHandlers;
		static const int _maxEpollEvents = 32;
		bool _gotStopSignal;

		void _initListeningSocket();// void initSockets();
		void _initEpoll();
		int _makeNewSocket(const std::string &ip, const std::string &port);
		void _createNewConnection();
		void _closeConnection(int connectionFd);
		void _delegateToConnectionHandler(int connectionFd);
		void handleConnectionEvent(int connectionFd, uint32_t events);
		bool isListeningSocket(int fd);
		void setNewHandeler(int newSocketFD);

	public:
		// HTTPServer(const MyConfig &_myConfig);
		HTTPServer(); // test
		HTTPServer(const HTTPServer &other);
		HTTPServer &operator=(const HTTPServer &other);
		~HTTPServer();

		void init();
		void start();
		void stop(int signal);
};
