
#include "../../../inc/connection_handler/HTTP_actions/HTTPActionGET.hpp"
#include "../../../inc/config_classes/ServerConfig.hpp"
#include "../../../inc/connection_handler/HTTPRequest.hpp"
#include "../../../inc/connection_handler/HTTPResponse.hpp"
#include <unistd.h>
#include <fstream>
#include <iostream>

HTTPActionGET::HTTPActionGET()
{ }

HTTPActionGET::~HTTPActionGET()
{ }

void HTTPActionGET::_fetchFile(HTTPRequest &request,
		HTTPResponse & response, 
		const ServerConfig &serverConfig)
{
	std::cout << "PATH: " << serverConfig.root + request.getRequestTarget() << std::endl;// test
	response.setBodyFromFile(serverConfig.root + request.getRequestTarget());
}

void HTTPActionGET::implementMethod(HTTPRequest &request,
		HTTPResponse & response, 
		const ServerConfig &serverConfig)
{	
	this->_fetchFile(request, response, serverConfig);
	
    // response.setBodyFromFile("tempFile.html");//test
	(void)request;//test
	(void)response;//test
	(void)serverConfig;//test
}

AMethod *HTTPActionGET::create()
{
	return (new HTTPActionGET());
}
