
#include "../inc/connection_handler/ConnectionHandler.hpp"
#include <unistd.h>
#include <sys/socket.h>
#include <cstring>

// ConnectionHandler::ConnectionHandler()
// {
// 	_connectionSocketFD = -1;
// 	// _HTTPAction = NULL;
// } // doet moelijk met &_serverKey

ConnectionHandler::ConnectionHandler(std::string &serverKey, int fd):
	_AHTTPAction(NULL),
	_serverKey(serverKey),
	_connectionSocketFD(fd)
{ }

ConnectionHandler::ConnectionHandler(const ConnectionHandler &other):
	_AHTTPAction(NULL),
	_serverKey(other._serverKey),//dit ok? 
	_connectionSocketFD(other._connectionSocketFD)
{ }

// ConnectionHandler &ConnectionHandler::operator=(const ConnectionHandler &other)
// {
// 	_connectionSocketFD = other._connectionSocketFD;
// 	return (*this);
// }

ConnectionHandler::~ConnectionHandler()
{
	if (_connectionSocketFD != -1)
		close(_connectionSocketFD);
}

int ConnectionHandler::_getConnectionSocketFD()
{
	return (_connectionSocketFD);
}

void ConnectionHandler::_createRequest()
{
	char buffer[2048]; // ?

	ssize_t bytesRead = recv(_connectionSocketFD, buffer, 2048 - 1, 0);

	if (bytesRead <= 0)
	{
		if (bytesRead == 0)
			std::cout << "Client closed connection " << _connectionSocketFD << std::endl;
		else if (bytesRead == -1)
			//TODO: 
			std::cerr << "Error reading from socket " << _connectionSocketFD << ": " << strerror(errno) << std::endl;
		// TODO: set flag to trigger isAutoclose()
		return;
	}

	buffer[bytesRead] = '\0';
	std::string rawRequest(buffer, bytesRead);

	std::cout << "Received " << bytesRead << " bytes from connection " << _connectionSocketFD << std::endl;
	std::cout << "Request: " << rawRequest.substr(0, 100) << "..." << std::endl;
	
	_request.parseRequest(rawRequest);
}

void ConnectionHandler::_sendResponse()
{
	//TODO: check if a response also has a max length
	_response.buildResponse();
	const std::string &responseString = _response.getResponseString();

	std::cout << "Sent back by server: " << responseString << std::endl; //test

	send(_connectionSocketFD, responseString.c_str(), responseString.length(), 0);
}


bool ConnectionHandler::shouldClose() 
{
	return (this->_request.hasCloseHeader());
	// ....
}

// void ConnectionHandler::makeResponse(std::string &input)
// {
// 	// std::cout << "input: " << input << std::endl;//test
// 	_request.parseRequest(input);
// 	std::cout << std::endl;
// 	// actions aanmaken ...
// }

void ConnectionHandler::_setServerConfig()
{
	_serverConfig = MyConfig::getServerConfig(_serverKey, _request.getHostURL());
}

void ConnectionHandler::handleHTTP()
{
	_response.reset();
	// make new Action, based on type of request
	this->_createRequest();
	this->_setServerConfig();
	// if !(_response.error)
		//TODO: use fitting Action
	this->_sendResponse();
}
