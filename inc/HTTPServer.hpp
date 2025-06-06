
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

		// void _initListeningSockets();
		void _initSockets();
		void _initEpoll();
		int _makeNewSocket(const std::string &ip, const std::string &port);
		void _createNewConnection(int fd);
		void _closeConnection(int connectionFd);
		void _delegateToConnectionHandler(int connectionFd);
		void _handleConnectionEvent(int connectionFd, uint32_t events);
		bool _isListeningSocket(int fd);
		void _setNewHandler(int newSocketFD);

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
