
#include "../../../inc/connection_handler/HTTP_actions/HTTPActionGET.hpp"
#include "../../../inc/config_classes/ServerConfig.hpp"
#include "../../../inc/connection_handler/HTTPRequest.hpp"
#include "../../../inc/connection_handler/HTTPResponse.hpp"
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>

HTTPActionGET::HTTPActionGET()
{ }

HTTPActionGET::~HTTPActionGET()
{ }

void HTTPActionGET::_fetchFile(HTTPRequest &request,
		HTTPResponse & response, 
		const ServerConfig &serverConfig)
{
	std::cout << "PATH: " 
		<< serverConfig.getFullPath(request.getRequestTarget()) 
		<< std::endl;// test
	response
		.setBodyFromFile(serverConfig
				.getFullPath(request
					.getRequestTarget()));
	//TODO: als file niet bestaat, eerst kijken of er een index bestaat? of enkel als laatste teken van path / is?
}

void HTTPActionGET::implementMethod(HTTPRequest &request,
		HTTPResponse & response, 
		const ServerConfig &serverConfig)
{	
	// if (std::find(serverConfig.getCurentRoute().allowedMethods.begin(),
	// 			serverConfig.getCurentRoute().allowedMethods.end(), "GET") 
	// 			== serverConfig.getCurentRoute().allowedMethods.end())
	if (!serverConfig.isAllowedMethod("GET"))
		response.setStatusCode(405);
	else
		this->_fetchFile(request, response, serverConfig);
}

AMethod *HTTPActionGET::create()
{
	return (new HTTPActionGET());
}
