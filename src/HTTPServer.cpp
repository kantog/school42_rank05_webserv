
#include "../inc/HTTPServer.hpp"
#include "../inc/MyConfig.hpp"

#include <asm-generic/socket.h>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <cstring>


#define MAX_LISTEN_QUEUE 10

HTTPServer::HTTPServer() : _listeningSocketFD(-1),
						   _epollFD(-1),
						   _connAmount(0)
// is this good practice, using a singleton like this basically creates a global?
{
}

HTTPServer::HTTPServer(const HTTPServer &other) : _listeningSocketFD(-1),
												  _epollFD(-1),
												  _connAmount(0)
{
	(void)other; // test
}

HTTPServer &HTTPServer::operator=(const HTTPServer &other)
{
	_listeningSocketFD = -1;
	_epollFD = -1;
	_connAmount = 0;
	(void)other; // test
	return (*this);
}

HTTPServer::~HTTPServer()
{
	std::map<int, ConnectionHandler>::iterator it;
	for (it = _connectionHandlers.begin(); it != _connectionHandlers.end(); ++it)
	{
		close(it->first);
	}
	_connectionHandlers.clear();

	if (_listeningSocketFD > 0)
		close(_listeningSocketFD);
	if (_epollFD > 0)
		close(_epollFD);
}

void HTTPServer::initListeningSocket()
{

	// TODO: meerdere severs nog te inplemneteren
	int serverNumber = 0;
	struct sockaddr_in socketAddress;
	int optval = 1;

	socketAddress.sin_family = AF_INET;
	socketAddress.sin_port = htons(MyConfig::getPort(serverNumber));
	socketAddress.sin_addr.s_addr = INADDR_ANY;

	_listeningSocketFD = socket(AF_INET, SOCK_NONBLOCK | SOCK_STREAM, 0); // SOCK_NONBLOCK is Linux specific, use fnctl for other systems
	if (_listeningSocketFD == -1)
		throw(std::runtime_error("Error: problem with socket creation"));
	if (setsockopt(_listeningSocketFD, SOL_SOCKET, SO_REUSEADDR,
				   &optval, sizeof(optval)) == -1)
		throw(std::runtime_error("Error setting socket options SO_REUSEADDR"));
	if (bind(_listeningSocketFD, (struct sockaddr *)&socketAddress, sizeof(socketAddress)) == -1)
		throw(std::runtime_error("Error: problem with socket binding"));
	if (listen(_listeningSocketFD, MAX_LISTEN_QUEUE) == -1)
		throw(std::runtime_error("Error listening for new connection"));
}

void HTTPServer::initEpoll()
{
	struct epoll_event localEpollEvent;

	_epollFD = epoll_create(1); // epoll_create argument is ignored since linux 2.6.8, but must be > 0
	if (_epollFD == -1)
		throw(std::runtime_error("Error: problem with epoll_create"));

	localEpollEvent.events = EPOLLIN | EPOLLET;
	localEpollEvent.data.fd = _listeningSocketFD;

	if (epoll_ctl(_epollFD, EPOLL_CTL_ADD, _listeningSocketFD, &localEpollEvent) == -1)
		throw(std::runtime_error("Error: problem with adding listeningSocketFD"));
}

void HTTPServer::init()
{
	std::cout << "Setting up server..." << std::endl;

	initListeningSocket();
	initEpoll();

	std::cout << "Server all set!" << std::endl;
}

void HTTPServer::createNewConnection()
{
	struct sockaddr_in socketAddress;
	socklen_t addressLen = sizeof(socketAddress);
	int newSocketFD = -1;

	std::cout << "Connecting..." << std::endl;

	newSocketFD = accept(_listeningSocketFD,
						 (struct sockaddr *)&socketAddress, &addressLen);
	// dit nog non-blocking maken? doet accept hetzelfde als socket()?
	if (newSocketFD == -1)
		throw(std::runtime_error("Error accepting new connection socket"));

	/*
		if (newSocketFD == -1) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			// No more connections to accept
			return;
		}
		throw (std::runtime_error("Error accepting new connection socket"));
	}
	*/
	// recv(client_fd, buffer, sizeof(buffer), 0) // read the actual request made by the client?

	// non blocking
	int flags = fcntl(newSocketFD, F_GETFL, 0);
	if (flags == -1 || fcntl(newSocketFD, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		close(newSocketFD);
		throw(std::runtime_error("Error: problem setting client socket to non-blocking"));
	}

	struct epoll_event newLocalEpollEvent;

	newLocalEpollEvent.events = EPOLLIN | EPOLLET;
	newLocalEpollEvent.data.fd = newSocketFD;

	if (epoll_ctl(_epollFD, EPOLL_CTL_ADD, newSocketFD,
				  &newLocalEpollEvent) == -1)
		throw(std::runtime_error("Error: problem with epoll_ctl"));

	ConnectionHandler newConnection(newSocketFD);
	_connectionHandlers[newSocketFD] = newConnection;
	_connAmount++;

	// sendResponse();// answer the request with a response?

	std::cout << "Connection established!" << std::endl;

	if (listen(_listeningSocketFD, MAX_LISTEN_QUEUE) == -1)
		throw(std::runtime_error("Error listening for new connection"));
}

void HTTPServer::handleConnection(int connectionFd)
{

	std::cout << "Handling connection " << connectionFd << std::endl;

	std::map<int, ConnectionHandler>::iterator it = _connectionHandlers.find(connectionFd);
	if (it == _connectionHandlers.end())
	{
		std::cerr << "Error: Connection handler not found for FD " << connectionFd << std::endl;
		return;
	}

	ConnectionHandler &connectionHandler = it->second;
	char buffer[2048]; // ?

	ssize_t bytesRead = recv(connectionFd, buffer, 2048 - 1, 0);

	if (bytesRead <= 0)
	{
		if (bytesRead == 0)
			std::cout << "Client closed connection " << connectionFd << std::endl;
		else if (errno != EAGAIN && errno != EWOULDBLOCK)
			std::cerr << "Error reading from socket " << connectionFd << ": " << strerror(errno) << std::endl;
		closeConnection(connectionFd);
		return;
	}

	buffer[bytesRead] = '\0';
	std::string raw_request(buffer, bytesRead);

	std::cout << "Received " << bytesRead << " bytes from connection " << connectionFd << std::endl;
	std::cout << "Request: " << raw_request.substr(0, 100) << "..." << std::endl;

	connectionHandler.makeResponse(raw_request);
	std::string response = connectionHandler.getResponse();

	send(connectionFd, response.c_str(), response.length(), 0);

	if (connectionHandler.isAutoClose())
		closeConnection(connectionFd);
}

void HTTPServer::closeConnection(int connectionFd)
{
	epoll_ctl(_epollFD, EPOLL_CTL_DEL, connectionFd, NULL);
	close(connectionFd);

	std::map<int, ConnectionHandler>::iterator it = _connectionHandlers.find(connectionFd);
	if (it != _connectionHandlers.end())
	{
		_connectionHandlers.erase(it);
		_connAmount--;
	}

	std::cout << "Connection " << connectionFd << " closed" << std::endl;
}

void HTTPServer::start()
{
	struct epoll_event localEpollEvents[_maxEpollEvents];
	int eventCount = 0;

	std::cout << "Waiting for a connection <3" << std::endl;

	while (true)
	{
		eventCount = epoll_wait(_epollFD, localEpollEvents, _maxEpollEvents, 0); // timeout is 0?
		if (eventCount == -1)
			throw(std::runtime_error("Error: problem while waiting for events"));
		if (eventCount > 0)						  // test
			std::cout << eventCount << std::endl; // test
		for (int i = 0; i < eventCount; i++)
		{
			int fd = localEpollEvents[i].data.fd;
			uint32_t events = localEpollEvents[i].events;

			std::cout << "Event on FD " << fd << " (listening FD is " << _listeningSocketFD << ")" << std::endl;

			if (fd == _listeningSocketFD)
			{
				std::cout << "New connection event" << std::endl;
				createNewConnection();
			}
			else
			{
				std::cout << "Data event on existing connection" << std::endl;

				if (events & (EPOLLHUP | EPOLLERR))
				{
					std::cout << "Connection error/hangup on FD " << fd << std::endl;
					closeConnection(fd);
				}
				else if (events & EPOLLIN)
				{
					handleConnection(fd);
				}
			}
		}
	}

	std::cout << "Server shutting down..." << std::endl;
}

void HTTPServer::stop()
{
}
