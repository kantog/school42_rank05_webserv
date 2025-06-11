
#include "../../../inc/connection_handler//HTTP_actions/HTTPActionPOST.hpp"

HTTPActionPOST::HTTPActionPOST()
{ }

HTTPActionPOST::~HTTPActionPOST()
{ }

void HTTPActionPOST::implementMethod(HTTPRequest &request,
						HTTPResponse & response, 
						const ServerConfig &serverConfig)
{
		(void)request;//test
		(void)response;//test
		(void)serverConfig;//test
}

AMethod *HTTPActionPOST::create()
{
	return (new HTTPActionPOST());
}
