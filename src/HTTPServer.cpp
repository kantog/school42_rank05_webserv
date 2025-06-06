
#include "../inc/HTTPServer.hpp"
#include "../inc/config_classes/MyConfig.hpp"

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
	//gewoon error throwen?
	_listeningSocketFD = -1;
	_epollFD = -1;
	_connAmount = 0;
	(void)other; // test
	return (*this);
}

HTTPServer::~HTTPServer()
{
	std::map<int, ConnectionHandler *>::iterator it;
	for (it = _connectionHandlers.begin(); it != _connectionHandlers.end(); ++it)
	{
		delete it->second; 
		close(it->first);
	}
	_connectionHandlers.clear();
	//
	if (_listeningSocketFD > 0)
		close(_listeningSocketFD);
	if (_epollFD > 0)
		close(_epollFD);
}

void HTTPServer::_initListeningSocket()
{

	// TODO: meerdere severs nog te inplemneteren
	int serverNumber = 0;
	struct sockaddr_in sockAdress;
	int optval = 1;

	sockAdress.sin_family = AF_INET;
	sockAdress.sin_port = htons(MyConfig::getPort(serverNumber));
	sockAdress.sin_addr.s_addr = INADDR_ANY;

	_listeningSocketFD = socket(AF_INET, SOCK_NONBLOCK | SOCK_STREAM, 0); // SOCK_NONBLOCK is Linux specific, use fnctl for other systems
	if (_listeningSocketFD == -1)
		throw(std::runtime_error("Error: problem with socket creation"));
	if (setsockopt(_listeningSocketFD, SOL_SOCKET, SO_REUSEADDR,
				   &optval, sizeof(optval)) == -1)
		throw(std::runtime_error("Error setting socket options SO_REUSEADDR"));
	if (bind(_listeningSocketFD, (struct sockaddr *)&sockAdress, sizeof(sockAdress)) == -1)
		throw(std::runtime_error("Error: problem with socket binding"));
	if (listen(_listeningSocketFD, MAX_LISTEN_QUEUE) == -1)
		throw(std::runtime_error("Error listening for new connection"));
}

void HTTPServer::_initEpoll()
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

	_initListeningSocket();
	_initEpoll();

	std::cout << "Server all set!" << std::endl;
}

void HTTPServer::_createNewConnection()
{
	struct sockaddr_in sockAdress;
	socklen_t addressLen = sizeof(sockAdress);
	int newSocketFD = -1;

	std::cout << "Connecting..." << std::endl;

	newSocketFD = accept(_listeningSocketFD, (struct sockaddr *)&sockAdress, &addressLen);
	if (newSocketFD == -1) 
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			std::cout << "No more connections to accept (EAGAIN/EWOULDBLOCK)" << std::endl;
			return;
		}
		std::cerr << "Accept failed: " << strerror(errno) << std::endl;
		throw (std::runtime_error("Error accepting new connection socket"));
	}

	std::cout << "Accepted new connection with FD: " << newSocketFD << std::endl;

	// Set non-blocking
	// int flags = fcntl(newSocketFD, F_GETFL, 0); TODO: GETFL mag niet?
	if (fcntl(newSocketFD, F_SETFL,  O_NONBLOCK) == -1)
	{
		close(newSocketFD);
		throw(std::runtime_error("Error: problem setting client socket to non-blocking"));
	}

	// Add to epoll
	struct epoll_event newLocalEpollEvent;
	newLocalEpollEvent.events = EPOLLIN | EPOLLET;
	newLocalEpollEvent.data.fd = newSocketFD;
	if (epoll_ctl(_epollFD, EPOLL_CTL_ADD, newSocketFD, &newLocalEpollEvent) == -1)
	{
		close(newSocketFD);//necessary? ConnectionHandler handles this?
		throw(std::runtime_error("Error: problem with epoll_ctl"));
	}

	// Create connection handler
	ConnectionHandler *newConnection = new ConnectionHandler(newSocketFD);
	_connectionHandlers[newSocketFD] = newConnection;
	_connAmount++;

	std::cout << "Connection established! FD=" << newSocketFD << ", Total connections: " << _connAmount << std::endl;

	this->_delegateToConnectionHandler(newSocketFD);
}

void HTTPServer::_delegateToConnectionHandler(int connectionFd)
{
	std::cout << "Handling connection " << connectionFd << std::endl;

	std::map<int, ConnectionHandler *>::iterator it = _connectionHandlers.find(connectionFd);
	if (it == _connectionHandlers.end())
	{
		std::cerr << "Error: Connection handler not found for FD " << connectionFd << std::endl;
		return;
	}
	ConnectionHandler *connectionHandler = it->second;
	connectionHandler->handleHTTP();
	if (connectionHandler->shouldClose())//TODO: Added this for cases where header says "connection close". 
										 //IF there are other cases where we should close after handling request, add to shouldClose()
		_closeConnection(connectionFd);
}

void HTTPServer::_closeConnection(int connectionFd)
{
	epoll_ctl(_epollFD, EPOLL_CTL_DEL, connectionFd, NULL);

	std::map<int, ConnectionHandler *>::iterator it = _connectionHandlers.find(connectionFd);
	if (it != _connectionHandlers.end())
	{
		delete it->second;
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
		eventCount = epoll_wait(_epollFD, localEpollEvents, _maxEpollEvents, -1); 
		if (eventCount == -1)
			throw(std::runtime_error("Error: problem while waiting for events"));
		if (eventCount > 0)						  // test
			std::cout << "eventCount: " << eventCount << std::endl; // test
		for (int i = 0; i < eventCount; i++)
		{
			int fd = localEpollEvents[i].data.fd;
			uint32_t events = localEpollEvents[i].events;

			std::cout << "Event on FD " << fd << " (listening FD is " << _listeningSocketFD << ")" << std::endl;

			if (fd == _listeningSocketFD)
			{
				std::cout << "New connection event" << std::endl;
				_createNewConnection();
			}
			else
			{
				std::cout << "Data event on existing connection" << std::endl;

				if (events & (EPOLLHUP | EPOLLERR))
				{
					std::cout << "Connection error/hangup on FD " << fd << std::endl;
					_closeConnection(fd);
				}
				else if (events & EPOLLIN)
					_delegateToConnectionHandler(fd);
			}
		}
	}

	std::cout << "Server shutting down..." << std::endl;
}

void HTTPServer::stop()
{
}
