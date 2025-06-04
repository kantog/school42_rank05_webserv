
#include "../inc/ConnectionHandler.hpp"
#include <unistd.h>

ConnectionHandler::ConnectionHandler()
{
	_connectionSocketFD = -1;
}

ConnectionHandler::ConnectionHandler(int input):
	_connectionSocketFD(input)
{
		
}

ConnectionHandler::ConnectionHandler(const ConnectionHandler &other):
	_connectionSocketFD(other._connectionSocketFD)
{
		
}

ConnectionHandler &ConnectionHandler::operator=(const ConnectionHandler &other)
{
	_connectionSocketFD = other._connectionSocketFD;
	return (*this);
}

ConnectionHandler::~ConnectionHandler()
{
	if (_connectionSocketFD !=-1)
		close(_connectionSocketFD);
}

// void ConnectionHandler::setConnectionSocketFD(int input)
// {
// 	_connectionSocketFD = input;
// }

int ConnectionHandler::getConnectionSocketFD()
{
	return (_connectionSocketFD);
}

// HTTPRequest ConnectionHandler::receiveRequest()
// {
//
// }

// void ConnectionHandler::sendResponse(int socketFD, HTTPResponse finishedHTTPResponse)
// {
//
// }


bool ConnectionHandler::isAutoClose()
{
	return (false); //////
}


void ConnectionHandler::makeResponse(std::string &input)
{
	std::cout << input << std::endl;
	_request.parseRequest(input);
	// actions aanmaken ...
}

std::string ConnectionHandler::getResponse()
{
	std::string response = "HTTP/1.1 200 OK\r\n\r\n Hello World";
	return (response);
}