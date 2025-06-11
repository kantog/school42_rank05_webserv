
#include "../inc/connection_handler//HTTP_actions/HTTPActionDEL.hpp"

HTTPActionDEL::HTTPActionDEL()
{ }

HTTPActionDEL::~HTTPActionDEL()
{ }

void HTTPActionDEL::implementMethod(HTTPRequest &request,
						HTTPResponse & response, 
						const ServerConfig &serverConfig)
{
		(void)request;//test
		(void)response;//test
		(void)serverConfig;//test
}

AMethod *HTTPActionDEL::create()
{
	return (new HTTPActionDEL());
}
