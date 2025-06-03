
#include "../inc/ConnectionHandler.hpp"
#include <unistd.h>

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
