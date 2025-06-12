
#include "../../../inc/connection_handler/HTTP_actions/HTTPActionPOST.hpp"
#include "../../../inc/config_classes/ServerConfig.hpp"
#include "../../../inc/connection_handler/HTTPRequest.hpp"
#include "../../../inc/connection_handler/HTTPResponse.hpp"

HTTPActionPOST::HTTPActionPOST()
{ }

HTTPActionPOST::~HTTPActionPOST()
{ }

void HTTPActionPOST::implementMethod(HTTPRequest &request,
						HTTPResponse & response, 
						const ServerConfig &serverConfig)
{
    response.setBodyFromFile("tests/test.html");//test
	//TODO: return niet 200 maar 201
	(void)request;//test
	(void)response;//test
	(void)serverConfig;//test
}

AMethod *HTTPActionPOST::create()
{
	return (new HTTPActionPOST());
}
