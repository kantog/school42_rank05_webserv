
#include "../../inc/HTTPServer.hpp"
#include "../../inc/config_classes/MyConfig.hpp"

#include <iostream>

#include <asm-generic/socket.h> // SO_REUSEADDR, ...
#include <stdexcept>            // std::runtime_error
#include <sys/socket.h>         // socket
#include <sys/epoll.h>          // epoll
#include <netinet/in.h>         // struct sockaddr_in, htons
#include <cstring>              // memset
#include <netdb.h>              // getaddrinfo
#include <fcntl.h>              // non-blocking
#include <unistd.h>             // close

HTTPServer::HTTPServer() : _epollFD(-1),
                           _connAmount(0),
                           _gotStopSignal(false)
{
}

HTTPServer::HTTPServer(const HTTPServer &other) : _epollFD(-1),
                                                  _connAmount(0)
{
    (void)other; // test
}

HTTPServer &HTTPServer::operator=(const HTTPServer &other)
{
    // gewoon error throwen?
    _epollFD = -1;
    _connAmount = 0;
    (void)other; // test
    return (*this);
}

HTTPServer::~HTTPServer()
{
    std::cout << "Server shutting down..." << std::endl;
    std::cout << "Amount of connections: " << _connAmount << std::endl;

    std::map<int, ConnectionHandler *>::iterator it;
    for (it = _connectionHandlers.begin(); it != _connectionHandlers.end(); ++it)
    {
        std::cout << "\tClosing connection " << it->first << std::endl;
        delete it->second;
        close(it->first);
    }
    _connectionHandlers.clear();
    //
    std::cout << "Closing listening sockets..." << std::endl;
    for (int i = 0; i < (int)_listeningSockets.size(); ++i)
    {
        std::cout << "\tClosing listening socket " << _listeningSockets[i].second << std::endl;
        close(_listeningSockets[i].second);
    }
    std::cout << "Closing epoll..." << std::endl;
    if (_epollFD > 0)
        close(_epollFD);
}

void HTTPServer::_setNonBlocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        close(fd);
        throw(std::runtime_error("Error: problem setting client socket to non-blocking"));
    }
}

void HTTPServer::_addFDToEpoll(int fd)
{
    struct epoll_event newLocalEpollEvent;
    newLocalEpollEvent.events = EPOLLIN | EPOLLET;
    newLocalEpollEvent.data.fd = fd;
    if (epoll_ctl(_epollFD, EPOLL_CTL_ADD, fd, &newLocalEpollEvent) == -1)
    {
        close(fd); // necessary? ConnectionHandler handles this?
        throw(std::runtime_error("Error: problem with epoll_ctl"));
    }
}

// void HTTPServer::initListeningSocket()
// {

// 	struct sockaddr_in sockAdress;
// 	int optval = 1;

// 	sockAdress.sin_family = AF_INET;
// 	sockAdress.sin_port = htons(8080); ////////
// 	sockAdress.sin_addr.s_addr = INADDR_ANY;

// 	_listeningSocketFD = socket(AF_INET, SOCK_NONBLOCK | SOCK_STREAM, 0); // SOCK_NONBLOCK is Linux specific, use fnctl for other systems
// 	if (_listeningSocketFD == -1)
// 		throw(std::runtime_error("Error: problem with socket creation"));
// 	if (setsockopt(_listeningSocketFD, SOL_SOCKET, SO_REUSEADDR,
// 				   &optval, sizeof(optval)) == -1)
// 		throw(std::runtime_error("Error setting socket options SO_REUSEADDR"));
// 	if (bind(_listeningSocketFD, (struct sockaddr *)&sockAdress, sizeof(sockAdress)) == -1)
// 		throw(std::runtime_error("Error: problem with socket binding"));
// 	if (listen(_listeningSocketFD, MAX_LISTEN_QUEUE) == -1)
// 		throw(std::runtime_error("Error listening for new connection"));
// }

void HTTPServer::_initListeningSockets()
{
    std::cout << "Creating listening sockets..." << std::endl;
    const MyConfig &myConfig = MyConfig::get();
    for (std::map<std::string, std::vector<ServerConfig> >::const_iterator it = myConfig._servers.begin(); it != myConfig._servers.end(); ++it)
    {
        int fd = _makeNewListeningSocket(it->second[0].host, it->second[0].port);
        this->_addFDToEpoll(fd);
        _listeningSockets.push_back(std::pair<std::string, int>(it->first, fd)); // TODO: is dit nodig?localEpollEvent.data.fd);
    }
}

// /////////////////////////

static struct addrinfo *_getAddressInfo(const std::string &ip, const std::string &port)
{
    struct addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    // Adress Info
    const char *host = (ip == "0.0.0.0") ? NULL : ip.c_str();
    struct addrinfo *res = NULL;
    int result = getaddrinfo(host, port.c_str(), &hints, &res);
    if (result != 0)
    {
        throw std::runtime_error("Error: getaddrinfo failed: " + std::string(gai_strerror(result)));
    }
    return res;
}

static int _createSocket(const struct addrinfo *addrInfo)
{
    int socketFD = socket(addrInfo->ai_family, addrInfo->ai_socktype | SOCK_NONBLOCK, addrInfo->ai_protocol);
    if (socketFD == -1)
    {
        throw std::runtime_error("Error: Failed to create socket");
    }
    return socketFD;
}

static void _setSocketOptions(int socketFD)
{
    int optval = 1;
    if (setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
    {
        close(socketFD);
        throw std::runtime_error("Error: Failed to set socket options");
    }
}

static void _bindSocket(int socketFD, const struct addrinfo *addrInfo)
{
    if (bind(socketFD, addrInfo->ai_addr, addrInfo->ai_addrlen) == -1)
    {
        close(socketFD);
        throw std::runtime_error("Error: Failed to bind socket");
    }
}

static void _startListening(int socketFD)
{
    if (listen(socketFD, MAX_LISTEN_QUEUE) == -1)
    {
        close(socketFD);
        throw std::runtime_error("Error: Failed to listen on socket");
    }
}

int HTTPServer::_makeNewListeningSocket(const std::string &ip, const std::string &port)
{
    struct addrinfo *res = _getAddressInfo(ip, port);
    int socketFD = -1;

    for (struct addrinfo *p = res; p != NULL; p = p->ai_next)
    {
        try
        {
            socketFD = _createSocket(p);
            _setSocketOptions(socketFD);
            _bindSocket(socketFD, p);
            break; // Success!
        }
        catch (...)
        {
            if (socketFD != -1)
                close(socketFD);
            socketFD = -1;
            continue;
        }
    }

    freeaddrinfo(res);
    if (socketFD == -1)
    {
        throw std::runtime_error("Error: Failed to bind to any address");
    }

    std::cout << "Listening on http://" << (ip.empty() ? "0.0.0.0" : ip) << ":" << port << std::endl;

    _startListening(socketFD);
    return socketFD;
}

// //////////////////////////

void HTTPServer::_initEpoll()
{
    std::cout << "Creating epoll..." << std::endl;
    _epollFD = epoll_create(1); // epoll_create argument is ignored since linux 2.6.8, but must be > 0
    if (_epollFD == -1)
        throw(std::runtime_error("Error: problem with epoll_create"));
}

void HTTPServer::init()
{
    std::cout << "Setting up server..." << std::endl;

    _initEpoll();
    _initListeningSockets();

    std::cout << "Server all set!" << std::endl;
}
