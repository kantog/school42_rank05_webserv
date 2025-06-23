
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

<<<<<<< HEAD
	std::cout << _serverKey << ": "
			  << _request.getMethod() << " " << _request.getRawPath()
			  << "(" << _serverConfig->getFullFilesystemPath(_request.getRequestTarget()) << "): "
			  << _response.getStatusCode() << std::endl;
=======
	// std::cout << _request.getMethod() << " " << _request.getRawPath()
	// 		  << "(" << _serverConfig->getFullFilesystemPath(_request.getRequestTarget())<< "): "
	// 		  << _response.getStatusCode() << std::endl;
>>>>>>> origin/refactoring-and-decoupling

	// const std::string &responseString = _response.getResponseString();

	// std::cout << "Sent back by server: " << responseString << std::endl; //test
	ssize_t bytesWritten = send(_connectionSocketFD, responseString.c_str(), responseString.length(), 0);
	if (bytesWritten == -1)
	{
		std::cerr << "Error writing to socket " << _connectionSocketFD
				  << ": " << strerror(errno) << std::endl;
		_shouldClose = true;
		return; // TODO: basil error handling in orde?
	}
}

void ConnectionHandler::killCgi()
{
	if (!_cgi)
		return;
	delete _cgi;
	_cgi = NULL;
}

bool ConnectionHandler::shouldClose()
{
	return (this->_request.hasCloseHeader() || this->_shouldClose);
}

void ConnectionHandler::_setServerConfig()
{
	_serverConfig = MyConfig::getServerConfig(_serverKey, _request.getHostURL());
	_serverConfig->setCorrectRoute(this->_request.getRequestTarget());
}

<<<<<<< HEAD
bool ConnectionHandler::handleHTTP()
=======
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
>>>>>>> origin/refactoring-and-decoupling
{
	if (this->_cgi)
	{
		this->_createRequest();
		_response.reset();
		_response.setStatusCode(503);
		_response.setHeader("Retry-After", "5");
		_response.setBody("Server busy processing request");
		_response.buildResponse();
		this->_sendResponse();
		return (false);
	}

	this->_createRequest();
	if (this->_request.isError())
	{	// TODO: test met grote files
		this->_setServerConfig();
		this->_response.buildErrorPage(this->_request.getErrorCode(), this->_serverConfig->getErrorPagePath(this->_request.getErrorCode()));
		this->_sendResponse();
		this->_shouldClose = true;
		return (true);
	}
	if (!this->_request.isComplete())
		return (false);

	this->_setServerConfig();

<<<<<<< HEAD
	_response.reset();

	_HTTPAction = new HTTPAction(_request, _response, *_serverConfig); // TODO:? heap/stack?
	_HTTPAction->run();
	if (_HTTPAction->isCgiRunning())
		_cgi = _HTTPAction->getCgi();
	delete _HTTPAction;
=======
	// _response.reset();
	// _response.setHeader("Set-Cookie", _request.getHeader("Cookie")); // TODO: test cookies

	HTTPAction Action(_request, *_serverConfig);
	Action.run();
	if (Action.isCgiRunning())
		_cgi = Action.getCgi();
>>>>>>> origin/refactoring-and-decoupling

	if (this->_cgi)
		return (true);

	this->_sendResponse(Action.getFullResponseString());

	this->_request.reset();
	return (true);
}

<<<<<<< HEAD
void ConnectionHandler::sendCgiResponse()
{
	if (_cgi->getStatusCode() != 200) // TODO: 200
		this->_response.buildErrorPage(_cgi->getStatusCode(), _serverConfig->getErrorPagePath(_cgi->getStatusCode()));
	else
		this->_response.buildCgiPage(_cgi->getBody());
	this->_sendResponse();
	_cgi = NULL;
	this->_request.reset();
}
=======
>>>>>>> origin/refactoring-and-decoupling
