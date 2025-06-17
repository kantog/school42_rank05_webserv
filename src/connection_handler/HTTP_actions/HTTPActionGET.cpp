
#include "../../../inc/connection_handler/HTTPRequest.hpp"
#include "../../../inc/connection_handler/HTTPResponse.hpp"
#include "../../../inc/config_classes/ServerConfig.hpp"

#include <unistd.h>
#include <iostream>

#include "../../../inc/connection_handler/HTTP_actions/HTTPActionGET.hpp"
HTTPActionGET::HTTPActionGET()
{ }

HTTPActionGET::~HTTPActionGET()
{ }

void HTTPActionGET::_fetchFile(HTTPRequest &request,
		HTTPResponse & response, 
		const ServerConfig &serverConfig)
{
// 	std::cout << "PATH: " //test
// 		<< serverConfig.getFullPath(request.getRequestTarget()) //test
// 		<< std::endl;// test

	//TODO: als index gespecifieerd is de indexfile fetchen bij /
	//TODO: als die niet bestaat checken of autoindex off is
	//TODO: anders error 404 

	response
		.setBodyFromFile(serverConfig
				.getFullPath(request
					.getRequestTarget()));

}

void HTTPActionGET::implementMethod(HTTPRequest &request,
		HTTPResponse & response, 
		const ServerConfig &serverConfig)
{	
	this->_fetchFile(request, response, serverConfig);
}

AMethod *HTTPActionGET::create()
{
	return (new HTTPActionGET());
}
