
#include "../../inc/HTTPServer.hpp"
#include "../../inc/config_classes/MyConfig.hpp"

#include <cstring>      // for strerror
#include <sys/socket.h> // for socket
#include <netinet/in.h> // for sockaddr_in
#include <sys/epoll.h>  // for epoll
#include <unistd.h>     // for close, read, write
#include <cerrno>       // for errno
#include <iostream>

void HTTPServer::_addCgi(ConnectionHandler *connectionHandler)
{
	Cgi *cgi = connectionHandler->getCgi(); // BASIL
	const int *fd = cgi->getCgiFds();

	while (*fd)
	{
		this->_addFDToEpoll(*fd);
		_cgis[*fd] = connectionHandler;
		fd++;
	}
}

void HTTPServer::_prossesCgi(ConnectionHandler *connectionHandler)
{
	Cgi *cgi = connectionHandler->getCgi();
	if (cgi->processCgi())
		return;
	
	connectionHandler->sendCgiResponse();

	const int *fd = cgi->getCgiFds();
	this->_closeConnection(_cgis, fd[0]);
	if (fd[1])
		this->_closeConnection(_cgis, fd[1]);
	delete cgi;
}

void HTTPServer::_createNewConnection(int fd)
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

	this->_setNonBlocking(newSocketFD);
	this->_addFDToEpoll(newSocketFD);
	this->_setNewHandler(newSocketFD);
	std::cout << "Connection established! FD=" << newSocketFD << ", Total connections: " << _connAmount << std::endl;

	this->_delegateToConnectionHandler(newSocketFD);
}

void HTTPServer::_setNewHandler(int newSocketFD)
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

ConnectionHandler *HTTPServer::_getConnectionHandler(std::map<int, ConnectionHandler *> &map, int fd)
{
	std::map<int, ConnectionHandler *>::iterator it = map.find(fd);
	if (it == map.end())
		return NULL;
	return it->second;
}

void HTTPServer::_delegateToConnectionHandler(int connectionFd)
{
	ConnectionHandler *connectionHandler = _getConnectionHandler(_cgis, connectionFd);
	if (connectionHandler)
	{
		_prossesCgi(connectionHandler); // BASIL
		return;
	}
	connectionHandler = _getConnectionHandler(_connectionHandlers, connectionFd);
	if (!connectionHandler)
	{
		std::cerr << "Error: Connection handler not found for FD " << connectionFd << std::endl;
		return;
	}
	connectionHandler->handleHTTP();

	if (connectionHandler->isCgiRunning())
		_addCgi(connectionHandler);

	if (connectionHandler->shouldClose())//TODO: Added this for cases where header says "connection close". 
										 //IF there are other cases where we should close after handling request, add to shouldClose()
		_closeConnection(_connectionHandlers, connectionFd);
}

void HTTPServer::_closeConnection(std::map<int, ConnectionHandler *> &map, int fd)
{
	epoll_ctl(_epollFD, EPOLL_CTL_DEL, fd, NULL);

	std::map<int, ConnectionHandler *>::iterator it = map.find(fd);
	if (it != map.end())
	{
		map.erase(it);
		if (map == _connectionHandlers)
		{
			_connAmount--;
			delete it->second;
			std::cout << "Connection " << fd << " closed" << std::endl;
		}
	}
}

bool HTTPServer::_isListeningSocket(int fd)
{
	for (std::vector<std::pair<std::string, int> >::const_iterator it = _listeningSockets.begin(); it != _listeningSockets.end(); ++it)
	{
		if (it->second == fd)
			return true;
	}
	return false;
}

void HTTPServer::_handleConnectionEvent(int fd, uint32_t events)
{
	if (events & (EPOLLHUP | EPOLLERR))
	{
        ConnectionHandler* cgiHandler = _getConnectionHandler(_cgis, fd); // BASIL
        if (cgiHandler)
		{
            _prossesCgi(cgiHandler);
            return;
        }
		std::cout << "Connection error/hangup on FD " << fd << std::endl;
		_closeConnection(_connectionHandlers, fd);
	}
	else if (events & EPOLLIN)
	{
		_delegateToConnectionHandler(fd);
	}
}

void HTTPServer::start()
{
	std::vector<struct epoll_event> localEpollEvents(_maxEpollEvents);

	std::cout << "Waiting for a connection <3" << std::endl;

	while (!this->_gotStopSignal)
	{
		int eventCount = epoll_wait(_epollFD, localEpollEvents.data(), _maxEpollEvents, -1);
		if (eventCount == -1)
		{
			if (errno == EINTR)
				continue;
			throw std::runtime_error("Error: problem while waiting for events");
		}
		for (int i = 0; i < eventCount; i++)
		{
			int fd = localEpollEvents[i].data.fd;

			if (_isListeningSocket(fd))
				_createNewConnection(fd);
			else
			{
				uint32_t events = localEpollEvents[i].events;
				_handleConnectionEvent(fd, events);
			}
		}
	}
}

void HTTPServer::stop(int signal)
{
	(void)signal;
	_gotStopSignal = true;
}
