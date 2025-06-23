
#include "../../inc/connection_handler/ConnectionHandler.hpp"
#include "HTTP_actions/HTTPAction.hpp"
#include "../../inc/config_classes/MyConfig.hpp"

#include <unistd.h>
#include <sys/socket.h>
#include <cstring>
#include <iostream>
#include <cerrno>

// ConnectionHandler::ConnectionHandler()
// {
// 	_connectionSocketFD = -1;
// 	// _HTTPAction = NULL;
// } // doet moelijk met &_serverKey

ConnectionHandler::ConnectionHandler(std::string &serverKey, int fd) : _cgi(NULL),
																	   _serverConfig(NULL),
																	   _shouldClose(false),
																	   _serverKey(serverKey),
																	   _connectionSocketFD(fd)
{
}

ConnectionHandler::ConnectionHandler(const ConnectionHandler &other) : _serverKey(other._serverKey), // dit ok?
																	   _connectionSocketFD(other._connectionSocketFD)
{
}

ConnectionHandler::~ConnectionHandler()
{
	if (_connectionSocketFD != -1)
		close(_connectionSocketFD);
}

int ConnectionHandler::_getConnectionSocketFD()
{
	return (_connectionSocketFD);
}

void ConnectionHandler::_handleErrorRecv(int bytesRead)
{
	if (bytesRead == 0)
	{
		std::cout << "Client closed connection " << _connectionSocketFD << std::endl;
		_shouldClose = true;
		return;
	}
	// bytesRead == -1
	if (errno == EAGAIN || errno == EWOULDBLOCK)
		return;
	std::cerr << "Error reading from socket " << _connectionSocketFD
			  << ": " << strerror(errno) << std::endl;
	_shouldClose = true;
	return;
}

void ConnectionHandler::_createRequest()
{
	const size_t bufferSize = 4096;
	char buffer[bufferSize];

	while (true)
	{
		ssize_t bytesRead = recv(_connectionSocketFD, buffer, bufferSize - 1, 0);

		if (bytesRead > 0)
		{
			buffer[bytesRead] = '\0';
			_request.parseRequest(buffer, _serverKey);

			if (_request.isComplete())
				return;
		}
		else
		{
			this->_handleErrorRecv(bytesRead);
			return;
		}
	}
}

void ConnectionHandler::_sendResponse(const std::string &responseString)
{

	// std::cout << _request.getMethod() << " " << _request.getRawPath()
	// 		  << "(" << _serverConfig->getFullFilesystemPath(_request.getRequestTarget())<< "): "
	// 		  << _response.getStatusCode() << std::endl;

	// const std::string &responseString = _response.getResponseString();

	// std::cout << "Sent back by server: " << responseString << std::endl; //test

	send(_connectionSocketFD, responseString.c_str(), responseString.length(), 0);
}

bool ConnectionHandler::shouldClose()
{
	return (this->_request.hasCloseHeader() || this->_shouldClose);
	// ....
}

void ConnectionHandler::_setServerConfig()
{
	_serverConfig = MyConfig::getServerConfig(_serverKey, _request.getHostURL());
	_serverConfig->setCorectRoute(this->_request.getRequestTarget());
} // TODO: setCorectRoute niet meer nodig

void ConnectionHandler::sendCgiResponse()
{
	HTTPAction Action(_request, *_serverConfig);

	// TODO: error checking
	// this->_response.buildCgiPage(_cgi->getBody());
	this->_sendResponse(Action.getFullCgiResponseString());
	_cgi = NULL;
	this->_request.reset(); // TODO ?
}

void ConnectionHandler::handleHTTP()
{
	if (this->_cgi) // TODO: leze niks mee doen?
		return;

	this->_createRequest();
	if (!this->_request.isComplete())
		return;

	this->_setServerConfig();

	// _response.reset();
	// _response.setHeader("Set-Cookie", _request.getHeader("Cookie")); // TODO: test cookies

	HTTPAction Action(_request, *_serverConfig);
	Action.run();
	if (Action.isCgiRunning())
		_cgi = Action.getCgi();

	if (this->_cgi)
		return;

	this->_sendResponse(Action.getFullResponseString());

	this->_request.reset(); // TODO ?
}

