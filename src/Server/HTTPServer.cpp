
#include "../../inc/HTTPServer.hpp"

#include <cstring>		// for strerror
#include <sys/socket.h> // for socket
#include <netinet/in.h> // for sockaddr_in
#include <sys/epoll.h>	// for epoll
#include <unistd.h>		// for close, read, write
#include <cerrno>		// for errno
#include <iostream>

void HTTPServer::_addCgi(ConnectionHandler *connectionHandler)
{
	Cgi *cgi = connectionHandler->getCgi();
	const int *fd = cgi->getCgiFds();

	while (*fd)
	{
		this->_addFDToEpoll(*fd);
		_cgis[*fd] = connectionHandler;
		fd++;
	}
}

void HTTPServer::_processCgi(ConnectionHandler *connectionHandler)
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
	this->_setNewHandler(newSocketFD, fd);
	std::cout << "Connection established! FD=" << newSocketFD << ", Total connections: " << _connAmount << std::endl;

	this->_delegateToConnectionHandler(newSocketFD);
}

void HTTPServer::_setNewHandler(int newSocketFD, int serverFD)
{
	std::string serverKey;
	for (std::vector<std::pair<std::string, int> >::const_iterator it = _listeningSockets.begin();
		 it != _listeningSockets.end(); ++it)
	{
		if (it->second == serverFD)
		{
			serverKey = it->first;
			break;
		}
	}

	if (serverKey.empty())
	{
		std::cerr << "Error: Could not find server key for FD " << serverFD << std::endl;
		close(newSocketFD);
		return;
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

	ConnectionHandler *connectionHandler = _getConnectionHandler(_connectionHandlers, connectionFd);

	if (!connectionHandler)
	{
		std::cerr << "Error: Connection handler not found for FD " << connectionFd << std::endl;
		return;
	}

	connectionHandler->handleHTTP();

	if (connectionHandler->isCgiRunning())
		_addCgi(connectionHandler);

	if (connectionHandler->shouldClose())
	{
		_closeConnection(_connectionHandlers, connectionFd);
		return;
	}
	if (connectionHandler->epolloutShouldOpen)
	{
		this->_setEPOLLOUT(connectionFd, true);
	}
	if (connectionHandler->epolloutShouldClose)
	{
		this->_setEPOLLOUT(connectionFd, false);
		connectionHandler->epolloutShouldClose = false;
	}
}

void HTTPServer::_closeConnection(std::map<int, ConnectionHandler *> &map, int fd)
{
	if (_epollFD > 0)
	{
		if (epoll_ctl(_epollFD, EPOLL_CTL_DEL, fd, NULL) == -1)
		{
			if (errno != EBADF && errno != ENOENT)
			{
				std::cerr << "Warning: epoll_ctl DEL failed for FD " << fd
						  << ": " << strerror(errno) << std::endl;
			}
		}
	}

	std::map<int, ConnectionHandler *>::iterator it = map.find(fd);
	{
		if (&map == &_connectionHandlers)
		{
			if (it->second->isCgiRunning())
				it->second->killCgi();

			delete it->second;
			_connAmount--;
			std::cout << "Connection " << fd << " closed" << std::endl;
		}
		map.erase(it);
	}
	if (close(fd) == -1)
	{
		if (errno != EBADF)
			std::cerr << "Warning: close failed for FD " << fd
					  << ": " << strerror(errno) << std::endl;
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
	ConnectionHandler *cgiHandler = _getConnectionHandler(_cgis, fd);
	if (cgiHandler)
	{
		_processCgi(cgiHandler);
		return;
	}
	if (events & (EPOLLHUP | EPOLLERR))
	{
		std::cout << "Connection error/hangup on FD " << events << std::endl; // test fiks fd terug
		_closeConnection(_connectionHandlers, fd);							  // test
		int err = 0;														  // test
		socklen_t len = sizeof(err);										  // test
		getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &len);					  // test
		if (err != 0)
		{																 // test
			std::cerr << "Socket error: " << strerror(err) << std::endl; // test
		} // test
		else
		{
			std::cerr << "Socket hangup" << std::endl; // test
		} // test
	}
	else if (events & (EPOLLIN | EPOLLOUT))
		_delegateToConnectionHandler(fd);
	else
		std::cerr << "Error: unknown epoll event" << std::endl;
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
