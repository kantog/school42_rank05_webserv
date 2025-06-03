
#include "../inc/HTTPServer.hpp"
#include <asm-generic/socket.h>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>

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

void HTTPServer::initSocket() 
{
	struct sockaddr_in socketAddress;
	int optval = 1;

	socketAddress.sin_family = AF_INET;
	// socketAddress.sin_port = htons(_myConfig._servers[_connAmount].port);
	socketAddress.sin_port = htons(8080);//this is just for testing without config file
	socketAddress.sin_addr.s_addr = INADDR_ANY;

	_listeningSocketFD = socket(AF_INET, SOCK_NONBLOCK | SOCK_STREAM, 0); //SOCK_NONBLOCK is Linux specific, use fnctl for other systems
	if (_listeningSocketFD == -1)
		throw (std::runtime_error("Error: socket creation"));
	if (setsockopt(_listeningSocketFD, SOL_SOCKET, SO_REUSEADDR, 
				&optval, sizeof(optval)) == -1)
		throw (std::runtime_error("Error: setting socket options SO_REUSEADDR"));
	if (bind(_listeningSocketFD, (struct sockaddr *)&socketAddress, sizeof(socketAddress)) == -1)
		throw (std::runtime_error("Error: socket binding"));
	if (listen(_listeningSocketFD, 10) == -1)
		throw (std::runtime_error("Error: listening for new connection"));
}

void HTTPServer::initEpoll() 
{
	struct epoll_event localEpollEvent;
	localEpollEvent.data.ptr = (void*)"test"; //do we need this?
	localEpollEvent.events = EPOLLIN | EPOLLET;
	_epollFD = epoll_create(1); //epoll_create argument is ignored since linux 2.6.8, but must be > 0

	localEpollEvent.data.fd = _listeningSocketFD;
	if (epoll_ctl(_epollFD, EPOLL_CTL_ADD, _listeningSocketFD, &localEpollEvent) == -1)
		throw (std::runtime_error("Error: problem with adding listeningSocketFD"));

}

void HTTPServer::init()
{
	std::cout << "Setting up server..." << std::endl;
	initSocket();
	initEpoll();
	std::cout << "Server all set!" << std::endl;
}
	
void HTTPServer::start()
{
	struct epoll_event localEpollEvents[_maxEpollEvents];
	struct sockaddr_in socketAddress;
	socklen_t addressLen = sizeof(socketAddress);
	int eventCount = 0;
	int newSocketFD = -1;

	std::cout << "Waiting for a connection <3" << std::endl;
	while (true)
	{
		eventCount = epoll_wait(_epollFD, localEpollEvents, _maxEpollEvents, 0);
		if (eventCount == -1)
			throw (std::runtime_error("Error: problem while waiting for events"));
		for (int i = 0; i < eventCount; i++)
		{
			if (localEpollEvents[i].data.fd == _listeningSocketFD)
			{
				std::cout << "Connecting..." << std::endl;
				newSocketFD = accept(_listeningSocketFD, 
					(struct sockaddr *)&socketAddress, &addressLen);
				//recv(client_fd, buffer, sizeof(buffer), 0) // read the actual request made by the client
				if (newSocketFD == -1)
					throw (std::runtime_error("Error: accepting new connection socket"));
				ConnectionHandler newConnection(newSocketFD);
				_connectionHandlers.push_back(newConnection);
				localEpollEvents[i].data.fd = newSocketFD;
				if (epoll_ctl(_epollFD, EPOLL_CTL_ADD, newSocketFD, 
							&localEpollEvents[i]) == -1)
					throw (std::runtime_error("Error: problem with epoll_ctl"));
				_connAmount++;
				//sendResponse();// answer the request with a response 
				std::cout << "Connection established!" << std::endl;
			}
			// else if () // 
				// _connectionHandlers[_connAmount].receiveRequest(...);
				// _connectionHandlers[_connAmount].sendResponse(...);

		}
	}
}

void HTTPServer::stop()
{ 


}
