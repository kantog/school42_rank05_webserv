
#include "../../../inc/connection_handler/HTTPRequest.hpp"
#include "../../../inc/connection_handler/HTTPResponse.hpp"

#include "Cgi.hpp"

#include <unistd.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>

#include <unistd.h>
#include <iostream>

#include "../../../inc/connection_handler/HTTP_actions/HTTPActionGET.hpp"
HTTPActionGET::HTTPActionGET()
{
}

HTTPActionGET::~HTTPActionGET()
{
}


/* TODO: fiks locatie
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
*/

void HTTPActionGET::_fetchFile(HTTPRequest &request,
							   HTTPResponse &response,
							   const ServerConfig &serverConfig)
{
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
