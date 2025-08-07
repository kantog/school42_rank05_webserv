#include "../../inc/connection_handler/ConnectionHandler.hpp"
#include "Defines.hpp"
#include "HTTP_actions/HTTPAction.hpp"
#include "../../inc/config_classes/MyConfig.hpp"
#include "../../inc/connection_handler/HTTPResponse.hpp"

#include <stdexcept>
#include <unistd.h>
#include <sys/socket.h>
#include <cstring>
#include <iostream>
#include <cerrno>

ConnectionHandler::ConnectionHandler(std::string &serverKey, int fd) : _cgi(NULL),
	_serverConfig(NULL),
	_shouldClose(false),
	_serverKey(serverKey),
	_connectionSocketFD(fd),
	epolloutShouldOpen(false)
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
			_request.parseRequest(buffer, bytesRead, _serverKey);

			if (_request.isComplete() || _request.isError())
			{
				return;
			}
		}
		else
		{
			// if (errno == EAGAIN)
			// {
			// 	epolloutShouldOpen = true;
			// 	return;
			// }
			this->_handleErrorRecv(bytesRead);
			return;
		}
	}
}

void ConnectionHandler::_sendResponse(const std::string &responseString)
{
	ssize_t bytesWritten = send(_connectionSocketFD, responseString.c_str(), 
			responseString.length(), 0);
	if (bytesWritten == -1)
	{
		std::cerr << "Error writing to socket " << _connectionSocketFD
			<< ": " << strerror(errno) << std::endl;
		_shouldClose = true;
		return;
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

void ConnectionHandler::sendCgiResponse()
{
	HTTPResponse response;

	if (200 < _cgi->getStatusCode() && _cgi->getStatusCode() > 226)
		response.buildErrorPage(_cgi->getStatusCode(), _serverConfig->getErrorPagePath(_cgi->getStatusCode()));
	else
		response.buildCgiPage(_cgi->getBody());
	this->_sendResponse(response.getResponseString());
	_cgi = NULL;
	this->_request.reset();
}
// Handles a single client request:
// - Checks if a CGI is already running and sends a retry message if busy
// - Parses the request and finds the correct server config
// - Delegates to an HTTPAction (like GET/POST) to build the response
// - Sends back the response or waits if the CGI is still running

bool ConnectionHandler::processClientRequest()
{
	// If a CGI process is already running, return 503 (busy)
    if (isCgiBusy())
        return false;

	// Parse the incoming request and determine which server config to use
    initializeRequest();

    // If there's an error in the request, send error response and close
    if (handleRequestError())
        return true;

    if (!this->_request.isComplete())
        return false;
    // Handle the action (GET, POST, DELETE...) and send response
    executeAction();
    return true;
}

// ---- Helper Methods ----
// Checks if a CGI is already in progress and replies with a 503 if so
bool ConnectionHandler::isCgiBusy()
{    
    if (this->_cgi)
    {
        HTTPResponse response;
        response.setStatusCode(503);
        response.setHeader("Retry-After", "5");
        response.setBody("Server busy processing request");
        response.buildResponse();
        this->_sendResponse(response.getResponseString());
        return true; // CGI busy
    }
    return false;
}

// Parses the request line + headers and sets the correct server config
void ConnectionHandler::initializeRequest()
{
    this->_createRequest();
    this->_setServerConfig();
}

// Sends an error response if the request is malformed or invalid
bool ConnectionHandler::handleRequestError()
{
    HTTPAction Action(_request, *_serverConfig);

    if (this->_request.isError())
    {
        this->_sendResponse(Action.getFullErrorResponseString(this->_request.getErrorCode()));
        this->_shouldClose = true;
        return true; // error handled
    }
    return false;
}
// Executes the HTTP action (GET/POST/DELETE), or starts CGI if needed
void ConnectionHandler::executeAction()
{
    HTTPAction Action(_request, *_serverConfig);
    Action.run();

    if (Action.isCgiRunning())
        _cgi = Action.getCgi();

    if (!this->_cgi)
    {
        this->_sendResponse(Action.getFullResponseString());
        this->_request.reset();
    }
}