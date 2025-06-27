#include "../../../inc/connection_handler/HTTP_actions/HTTPActionOPTIONS.hpp"
#include "../../../inc/connection_handler/HTTPResponse.hpp"
#include "../../../inc/connection_handler/HTTPRequest.hpp"
#include "../../../inc/config_classes/ServerConfig.hpp"
#include "Defines.hpp"

#include <iostream>

// THIS METHOD IS INCLUDED ONLY TO TEST THE DELETE METHOD IN THE BROWSER.
// WITHOUT IT, THE CORS SECURITY SYSTEM WILL NOT ALLOW DELETE REQUESTS.
// IT IS NOT MEANT FOR ACTUAL USE.

HTTPActionOPTIONS::HTTPActionOPTIONS()
{ }

HTTPActionOPTIONS::~HTTPActionOPTIONS()
{ }

void HTTPActionOPTIONS::implementMethod(HTTPRequest &request,
						HTTPResponse & response, 
						const ServerConfig &serverConfig)
{
	response.setStatusCode(HTTP_OK_NOCONTENT);
	response.setHeader("Access-Control-Allow-Origin", "*");
	response.setHeader("Access-Control-Allow-Methods", "DELETE, OPTIONS");
	response.setHeader("Access-Control-Allow-Headers", "Content-Type");

	(void)request;
	(void)serverConfig;
}

AMethod *HTTPActionOPTIONS::create()
{
	std::cout << "TEST" << std::endl;//test
	return (new HTTPActionOPTIONS());
}
