
#include "../inc/connection_handler//HTTP_actions/HTTPActionGET.hpp"

HTTPActionGET::HTTPActionGET()
{ }

HTTPActionGET::~HTTPActionGET()
{ }

void HTTPActionGET::implementMethod(HTTPRequest &request,
						HTTPResponse & response, 
						const ServerConfig &serverConfig)
{
		(void)request;//test
		(void)response;//test
		(void)serverConfig;//test
}

AMethod *HTTPActionGET::create()
{
	return (new HTTPActionGET());
}
