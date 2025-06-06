
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
#include <netdb.h>

#define MAX_LISTEN_QUEUE 10

HTTPServer::HTTPServer() : _epollFD(-1),
						   _connAmount(0),
						   _GotStopSignal(false)
// is this good practice, using a singleton like this basically creates a global?
{
}

HTTPServer::HTTPServer(const HTTPServer &other) : _epollFD(-1),
												  _connAmount(0)
{
	(void)other; // test
}

HTTPServer &HTTPServer::operator=(const HTTPServer &other)
{
	// gewoon error throwen?
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
	for (int i = 0; i < (int)_listeningSockets.size(); ++i)
		close(_listeningSockets[i].second);
	if (_epollFD > 0)
		close(_epollFD);
}

void HTTPServer::initSockets()
{
	const MyConfig &myConfig = MyConfig::get();
	for (std::map<std::string, std::vector<ServerConfig> >::const_iterator it = myConfig._servers.begin(); it != myConfig._servers.end(); ++it)
	{
		struct epoll_event localEpollEvent;
		localEpollEvent.events = EPOLLIN | EPOLLET;
		localEpollEvent.data.fd = makeNewSocket(it->second[0].host, it->second[0].port);

		if (epoll_ctl(_epollFD, EPOLL_CTL_ADD, localEpollEvent.data.fd, &localEpollEvent) == -1)
			throw(std::runtime_error("Error: problem with adding listeningSocketFD"));

		_listeningSockets.push_back(std::pair<std::string, int>(it->first, localEpollEvent.data.fd)); // TODO: is dit nodig?localEpollEvent.data.fd);
	}
}

// void HTTPServer::initListeningSocket()
// {

// 	// TODO: meerdere severs nog te inplemneteren
// 	struct sockaddr_in sockAdress;
// 	int optval = 1;

// 	sockAdress.sin_family = AF_INET;
// 	sockAdress.sin_port = htons(8080); ////////
// 	sockAdress.sin_addr.s_addr = INADDR_ANY;

// 	_listeningSocketFD = socket(AF_INET, SOCK_NONBLOCK | SOCK_STREAM, 0); // SOCK_NONBLOCK is Linux specific, use fnctl for other systems
// 	if (_listeningSocketFD == -1)
// 		throw(std::runtime_error("Error: problem with socket creation"));
// 	if (setsockopt(_listeningSocketFD, SOL_SOCKET, SO_REUSEADDR,
// 				   &optval, sizeof(optval)) == -1)
// 		throw(std::runtime_error("Error setting socket options SO_REUSEADDR"));
// 	if (bind(_listeningSocketFD, (struct sockaddr *)&sockAdress, sizeof(sockAdress)) == -1)
// 		throw(std::runtime_error("Error: problem with socket binding"));
// 	if (listen(_listeningSocketFD, MAX_LISTEN_QUEUE) == -1)
// 		throw(std::runtime_error("Error listening for new connection"));
// }

int HTTPServer::makeNewSocket(const std::string &ip, const std::string &port)
{
	struct addrinfo hints;
	struct addrinfo *res = NULL;
	int optval = 1;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	const char *host = (ip == "0.0.0.0") ? NULL : ip.c_str();
	int gai_result = getaddrinfo(host, port.c_str(), &hints, &res);
	if (gai_result != 0)
	{
		std::string error_msg = "Error: getaddrinfo failed: ";
		error_msg += gai_strerror(gai_result);
		throw std::runtime_error(error_msg);
	}

	int socketFD = -1;
	for (struct addrinfo *p = res; p != NULL; p = p->ai_next)
	{
		socketFD = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (socketFD == -1)
			continue;
		if (setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
		{
			close(socketFD);
			socketFD = -1;
			continue;
		}
		int flags = fcntl(socketFD, F_GETFL, 0);
		if (flags == -1 || fcntl(socketFD, F_SETFL, flags | O_NONBLOCK) == -1)
		{
			close(socketFD);
			socketFD = -1;
			continue;
		}
		if (bind(socketFD, p->ai_addr, p->ai_addrlen) == 0)
			break; // Success!

		close(socketFD);
		socketFD = -1;
	}
	freeaddrinfo(res);
	if (socketFD == -1)
		throw std::runtime_error("Error: Failed to bind to any address");

	if (listen(socketFD, MAX_LISTEN_QUEUE) == -1)
	{
		close(socketFD);
		throw std::runtime_error("Error listening for new connection");
	}
	return socketFD;
}

void HTTPServer::initEpoll()
{
	_epollFD = epoll_create(1); // epoll_create argument is ignored since linux 2.6.8, but must be > 0
	if (_epollFD == -1)
		throw(std::runtime_error("Error: problem with epoll_create"));
}

void HTTPServer::init()
{
	std::cout << "Setting up server..." << std::endl;

	initEpoll();
	initSockets();

	std::cout << "Server all set!" << std::endl;
}

void HTTPServer::createNewConnection(int fd)
{
	struct sockaddr_in sockAdress;
	socklen_t addressLen = sizeof(sockAdress);
	int newSocketFD = -1;

	std::cout << "Connecting..." << std::endl;

	newSocketFD = accept(fd, (struct sockaddr *)&sockAdress, &addressLen);
	if (newSocketFD == -1)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
		{
			std::cout << "No more connections to accept (EAGAIN/EWOULDBLOCK)" << std::endl;
			return;
		}
		std::cerr << "Accept failed: " << strerror(errno) << std::endl;
		throw(std::runtime_error("Error accepting new connection socket"));
	}

	std::cout << "Accepted new connection with FD: " << newSocketFD << std::endl;

	// Set non-blocking
	// int flags = fcntl(newSocketFD, F_GETFL, 0); TODO: GETFL mag niet?
	if (fcntl(newSocketFD, F_SETFL, O_NONBLOCK) == -1)
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
		close(newSocketFD); // necessary? ConnectionHandler handles this?
		throw(std::runtime_error("Error: problem with epoll_ctl"));
	}

	this->setNewHandeler(newSocketFD);

	std::cout << "Connection established! FD=" << newSocketFD << ", Total connections: " << _connAmount << std::endl;

	this->delegateToConnectionHandler(newSocketFD);
}

void HTTPServer::setNewHandeler(int newSocketFD)
{
	std::string &serverKey = _listeningSockets[0].first;
	for (std::vector<std::pair<std::string, int> >::const_iterator it = _listeningSockets.begin(); it != _listeningSockets.end(); ++it)
	{
		if (it->second == newSocketFD)
		{
			serverKey = it->first;
			break;
		}
	}

	ConnectionHandler *newConnection = new ConnectionHandler(serverKey, newSocketFD);
	_connectionHandlers[newSocketFD] = newConnection;
	_connAmount++;
}

void HTTPServer::delegateToConnectionHandler(int connectionFd)
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
	if (connectionHandler->shouldClose()) // TODO: Added this for cases where header says "connection close".
										  // IF there are other cases where we should close after handling request, add to shouldClose()
		closeConnection(connectionFd);
}

void HTTPServer::closeConnection(int connectionFd)
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

bool HTTPServer::isListeningSocket(int fd)
{
	for (std::vector<std::pair<std::string, int> >::const_iterator it = _listeningSockets.begin(); it != _listeningSockets.end(); ++it)
	{
		if (it->second == fd)
			return true;
	}
	return false;
}

void HTTPServer::handleConnectionEvent(int fd, uint32_t events)
{
	std::cout << "Data event on existing connection" << std::endl;

	if (events & (EPOLLHUP | EPOLLERR))
	{
		std::cout << "Connection error/hangup on FD " << fd << std::endl;
		closeConnection(fd);
	}
	else if (events & EPOLLIN)
	{
		delegateToConnectionHandler(fd);
	}
}

void HTTPServer::start()
{
	std::vector<struct epoll_event> localEpollEvents(_maxEpollEvents);

	std::cout << "Waiting for a connection <3" << std::endl;

	while (!this->_GotStopSignal)
	{
		int eventCount = epoll_wait(_epollFD, localEpollEvents.data(), _maxEpollEvents, -1); // 10 second timeout?
		if (eventCount == -1)
		{
			if (errno == EINTR)
				continue;
			throw std::runtime_error("Error: problem while waiting for events");
		}
		for (int i = 0; i < eventCount; i++)
		{
			int fd = localEpollEvents[i].data.fd;
			uint32_t events = localEpollEvents[i].events;

			if (isListeningSocket(fd))
			{
				std::cout << "New connection event" << std::endl;
				createNewConnection(fd);
			}
			else
			{
				handleConnectionEvent(fd, events);
			}
		}
	}

	std::cout << "Server shutting down..." << std::endl;
}

void HTTPServer::stop(int signal)
{
	(void)signal;
	_GotStopSignal = true;
}
