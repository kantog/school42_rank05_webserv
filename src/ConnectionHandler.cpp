
#include "../inc/ConnectionHandler.hpp"
#include <unistd.h>
#include <sys/socket.h>
#include <cstring>

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
	if (_connectionSocketFD != -1)
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

void ConnectionHandler::receiveRequest()
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
		// closeConnection(connectionFd);
		return;
	}

	buffer[bytesRead] = '\0';
	std::string rawRequest(buffer, bytesRead);

	std::cout << "Received " << bytesRead << " bytes from connection " << _connectionSocketFD << std::endl;
	std::cout << "Request: " << rawRequest.substr(0, 100) << "..." << std::endl;
	
	_request.parseRequest(rawRequest);

	//to do: dit hieronder weghalen en sendResponse doen werken
	makeResponse(rawRequest);
	std::string response = getResponse(); // dit is gwn om te testen

	send(_connectionSocketFD, response.c_str(), response.length(), 0);

	// if (connectionHandler.isAutoClose())
	// 	closeConnection(connectionFd);
}
//
// void ConnectionHandler::sendResponse(int socketFD, HTTPResponse finishedHTTPResponse)
// {
// 	makeResponse(rawRequest);
// 	std::string response = getResponse();
//
// 	send(_connectionSocketFD, response.c_str(), response.length(), 0);
//
// 	if (connectionHandler.isAutoClose())
// 		closeConnection(connectionFd);
//
// }


bool ConnectionHandler::isAutoClose()
{
	return (this->_request.isAutoClose());
}


void ConnectionHandler::makeResponse(std::string &input)
{
	// std::cout << "input: " << input << std::endl;//test
	_request.parseRequest(input);
	std::cout << std::endl;
	// actions aanmaken ...
}

std::string ConnectionHandler::getResponse()
{
	std::string response = "HTTP/1.1 200 OK\r\n\r\n Hello World";
	return (response);
}

void ConnectionHandler::handleHTTP()
{
	this->receiveRequest();
	//interne handlerfunctie van connectionHandler?
	// this->sendResponse();
		
}
