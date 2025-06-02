
#include "../inc/HTTPServer.hpp"
#include <asm-generic/socket.h>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/epoll.h>


HTTPServer::HTTPServer(const MyConfig &myConfig): _myConfig(MyConfig::get())
//is this good practice, using a singleton like this basically creates a global?
{ }

HTTPServer::HTTPServer(const HTTPServer &other): _myConfig(other._myConfig)
{ }

HTTPServer &HTTPServer::operator=(const HTTPServer &other) 
{ return (*this); }

HTTPServer::~HTTPServer()
{ }

void HTTPServer::init()
{
	_socketFD = socket(AF_LOCAL, SOCK_NONBLOCK, 0);
	bind(_socketFD, _myConfig._servers.port, 32);
	setsockopt(_socketFD, SOL_SOCKET, SO_DEBUG | SO_REUSEADDR, const void *optval, socklen_t optlen)
	if (listen(_socketFD, 1);//1 connection requests or more?

	struct epoll_event local_epoll_event;
	local_epoll_event.data.ptr = (void*)"test"; //do we need this?
	local_epoll_event.events = EPOLLOUT | EPOLLIN;
	_epollFD = epoll_create(1); //argument is ignored since linux 2.6.8, but must be > 0
	if (epoll_ctl(_epollFD, EPOLL_CTL_ADD, _socketFD, &local_epoll_event) != 0)
		throw (std::runtime_error("Error: problem with epoll_ctl"));
}
	
void HTTPServer::start()
{
	// hier iets met ctl?

}

void HTTPServer::stop()
{ 

}
