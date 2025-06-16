
#include "../../../inc/connection_handler/HTTP_actions/HTTPActionGET.hpp"
#include "../../../inc/config_classes/ServerConfig.hpp"
#include "../../../inc/connection_handler/HTTPRequest.hpp"
#include "../../../inc/connection_handler/HTTPResponse.hpp"

#include "Cgi.hpp"

#include <unistd.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>

HTTPActionGET::HTTPActionGET()
{
}

HTTPActionGET::~HTTPActionGET()
{
}

void HTTPActionGET::_fetchFile(HTTPRequest &request,
							   HTTPResponse &response,
							   const ServerConfig &serverConfig)
{
	std::cout << "PATH: "
			  << serverConfig.getFullPath(request.getRequestTarget())
			  << std::endl; // test

	std::string path = serverConfig.getFullPath(request.getRequestTarget());

	if (serverConfig.isAllowedCgi(path))
	{
		Cgi cgi(request, serverConfig);
		cgi.startCgi();
		int code = cgi.getStatusCode();
		if (code != 200)
		{
			response.buildErrorPage(code, serverConfig.getErrorPagePath(code));
			return;
		}
		response.buildCgiPage(cgi.getBody());
	}
	else
		response.setBodyFromFile(path);
	// TODO: als index gespecifieerd is de indexfile fetchen bij /
	// TODO: als die niet bestaat checken of autoindex off is
	// TODO: anders error 404
}

void HTTPActionGET::implementMethod(HTTPRequest &request,
									HTTPResponse &response,
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
