
#include "../inc/HTTPServer.hpp"
#include <asm-generic/socket.h>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>

#define MAX_LISTEN_QUEUE 10

HTTPServer::HTTPServer(): //test
// HTTPServer::HTTPServer(const MyConfig &myConfig): //test uncomment
	// _myConfig(MyConfig::get()),//test uncomment
	_listeningSocketFD(-1),
	_epollFD(-1),
	_connAmount(0)
//is this good practice, using a singleton like this basically creates a global?
{ }

HTTPServer::HTTPServer(const HTTPServer &other):
	// _myConfig(other._myConfig), //test
	_listeningSocketFD(-1),
	_epollFD(-1),
	_connAmount(0)
{ 
	(void)other;//test
}

HTTPServer &HTTPServer::operator=(const HTTPServer &other) 
{ 
	_listeningSocketFD = -1;
	_epollFD = -1;
	_connAmount = 0;
	// _myConfig = other.myConfig; //test uncomment
	(void)other;//test
	return (*this); 
}

HTTPServer::~HTTPServer()
{ 
	if (_listeningSocketFD >= 0)
		close(_listeningSocketFD);
	if (_epollFD >= 0)
		close(_epollFD);
}

void HTTPServer::initListeningSocket() 
{
	struct sockaddr_in socketAddress;
	int optval = 1;

	socketAddress.sin_family = AF_INET;
	// socketAddress.sin_port = htons(_myConfig._servers[_connAmount].port);
	socketAddress.sin_port = htons(8080);//this is just for testing without config file
	socketAddress.sin_addr.s_addr = INADDR_ANY;//set to 127.0.0.1?

	_listeningSocketFD = socket(AF_INET, SOCK_NONBLOCK | SOCK_STREAM, 0); //SOCK_NONBLOCK is Linux specific, use fnctl for other systems
	if (_listeningSocketFD == -1)
		throw (std::runtime_error("Error: problem with socket creation"));
	if (setsockopt(_listeningSocketFD, SOL_SOCKET, SO_REUSEADDR, 
				&optval, sizeof(optval)) == -1)
		throw (std::runtime_error("Error setting socket options SO_REUSEADDR"));
	if (bind(_listeningSocketFD, (struct sockaddr *)&socketAddress, sizeof(socketAddress)) == -1)
		throw (std::runtime_error("Error: problem with socket binding"));
	if (listen(_listeningSocketFD, MAX_LISTEN_QUEUE) == -1)
		throw (std::runtime_error("Error listening for new connection"));
}

void HTTPServer::initEpoll() 
{
	struct epoll_event localEpollEvent;
	localEpollEvent.events = EPOLLIN | EPOLLET;
	_epollFD = epoll_create(1); //epoll_create argument is ignored since linux 2.6.8, but must be > 0

	localEpollEvent.data.fd = _listeningSocketFD;
	if (epoll_ctl(_epollFD, EPOLL_CTL_ADD, _listeningSocketFD, &localEpollEvent) == -1)
		throw (std::runtime_error("Error: problem with adding listeningSocketFD"));
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
		throw (std::runtime_error("Error accepting new connection socket"));

	//recv(client_fd, buffer, sizeof(buffer), 0) // read the actual request made by the client?

	struct epoll_event newLocalEpollEvent;
	newLocalEpollEvent.events = EPOLLIN | EPOLLET;
	if (epoll_ctl(_epollFD, EPOLL_CTL_ADD, newSocketFD, 
				&newLocalEpollEvent) == -1)
		throw (std::runtime_error("Error: problem with epoll_ctl"));

	ConnectionHandler newConnection(newSocketFD);
	_connectionHandlers.push_back(newConnection);
	_connAmount++;

	//sendResponse();// answer the request with a response?
	
	std::cout << "Connection established!" << std::endl;

	if (listen(_listeningSocketFD, MAX_LISTEN_QUEUE) == -1)
		throw (std::runtime_error("Error listening for new connection"));
}

void HTTPServer::start()
{
	struct epoll_event localEpollEvents[_maxEpollEvents];
	int eventCount = 0;

	std::cout << "Waiting for a connection <3" << std::endl;
	
	while (true)
	{
		eventCount = epoll_wait(_epollFD, localEpollEvents, _maxEpollEvents, 0);
		if (eventCount == -1)
			throw (std::runtime_error("Error: problem while waiting for events"));
		if (eventCount > 0)//test
			std::cout << eventCount << std::endl; //test 
		for (int i = 0; i < eventCount; i++)
		{
			if (localEpollEvents[i].data.fd == _listeningSocketFD)
				createNewConnection();
			// else zoek connectinHandler via _connectionFd
				// _connectionHandlers[_connAmount].receiveRequest(...);
				// _connectionHandlers[_connAmount].sendResponse(...);
				// 	=> in handleResponse: first set EPOLLOUT to the localEpollEvent,
				// 	afterwards turn it off again

		}
	}

	std::cout << "Server shutting down..." << std::endl;
}

void HTTPServer::stop()
{ 


}
