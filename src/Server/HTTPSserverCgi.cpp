#include "HTTPServer.hpp"
#include <iostream>


void HTTPServer::checkCgiTimeouts()
{
    // std::cout << "Checking for CGI timeouts..." << std::endl;
    std::vector<ConnectionHandler *> toKill;
    for (std::map<int, ConnectionHandler *>::iterator it = _cgis.begin(); it != _cgis.end(); ++it)
    {
        ConnectionHandler *handler = it->second;
        if (handler->isCgiRunning())
        {
            Cgi *cgi = handler->getCgi();
            if (cgi->isOverTimeLimit())
            {
                std::cout << "Killing CGI for connection " << it->first << " due to timeout." << std::endl;
                toKill.push_back(handler);
            }
        }
    }
    for (size_t i = 0; i < toKill.size(); ++i)
        _processCgi(toKill[i]); 

}

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