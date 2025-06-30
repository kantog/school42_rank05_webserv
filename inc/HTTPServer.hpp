
#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include "connection_handler/ConnectionHandler.hpp"

class HTTPServer
{
	private:
		HTTPServer(const HTTPServer &other);
		HTTPServer &operator=(const HTTPServer &other);

		std::vector<std::pair<std::string, int> > _listeningSockets;
		int _epollFD;
		int _connAmount;
		std::map<int, ConnectionHandler *> _connectionHandlers;
		std::map<int, ConnectionHandler *> _cgis;

		static const int _maxEpollEvents = 32;
		bool _gotStopSignal;

		void _initListeningSockets();
		void _initEpoll();

		void _setNonBlocking(int fd);
		void _addFDToEpoll(int fd);
		void _setEPOLLOUT(int fd, bool on);
		int _makeNewListeningSocket(const std::string &ip, const std::string &port);
		
		void _createNewConnections(int fd);
		void _closeConnection(std::map<int, ConnectionHandler *> &map, int fd);
		void _delegateToConnectionHandler(int connectionFd);
		void _delegateToCgi(int connectionFd);
		void _handleConnectionEvent(int connectionFd, uint32_t events);
		bool _isListeningSocket(int fd);
		void _setNewHandler(int newSocketFD, int serverFD);

		ConnectionHandler *_getConnectionHandler(std::map<int, ConnectionHandler *> &map, int fd);

		void _addCgi(ConnectionHandler *connectionHandler);
		void _processCgi(ConnectionHandler *connectionHandler);

	public:
		HTTPServer();
		~HTTPServer();

		void init();

		void start();
		void stop(int signal);

};
