
#pragma once

#include <string>
#include <vector>
#include <stdint.h>
#include "ConnectionHandler.hpp"


class HTTPServer
{
private:
	std::vector<int> _listeningSocketFDs;
	int _epollFD;
	int _connAmount;
	std::map<int, ConnectionHandler *> _connectionHandlers;
	static const int _maxEpollEvents = 32;

	void initSockets();
	int makeNewSocket(const std::string &ip, const std::string &port);
	void initEpoll();
	void createNewConnection(int fd);
	void closeConnection(int connectionFd);
	void delegateToConnectionHandler(int connectionFd);

	void handleConnectionEvent(int connectionFd, uint32_t events);
	bool isListeningSocket(int fd);

public:
	// HTTPServer(const MyConfig &_myConfig);
	HTTPServer(); // test
	HTTPServer(const HTTPServer &other);
	HTTPServer &operator=(const HTTPServer &other);
	~HTTPServer();

	void init();
	void start();
	void stop();
};
