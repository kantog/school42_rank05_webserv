#include "../../../inc/connection_handler//HTTP_actions/HTTPActionDEL.hpp"
#include "../../../inc/config_classes/ServerConfig.hpp"
#include "../../../inc/connection_handler/HTTPRequest.hpp"
#include "../../../inc/connection_handler/HTTPResponse.hpp"

#include <cstdio>
#include <errno.h>

HTTPActionDEL::HTTPActionDEL()
{ }

HTTPActionDEL::~HTTPActionDEL()
{ }

void HTTPActionDEL::implementMethod(HTTPRequest &request,
						HTTPResponse & response, 
						const ServerConfig &serverConfig)
{	
	if (request.getBody() != "")
	{
		response.setStatusCode(400);
		return;
	}
	errno = 0;
	int exitCode = std::remove(serverConfig.getFullPath(request.getRequestTarget()).c_str());
	if (exitCode != 0)
	{
		switch (errno)
		{
			case  EACCES: 	
				response.setStatusCode(403);
				return;
			case ENOENT:	
				response.setStatusCode(404);
				return;
			case EISDIR:	
				response.setStatusCode(409);
				return;
			default: 
				response.setStatusCode(500);
				return;
		}
	}
	response.setBody("Succes: " + request.getRequestTarget() + " deleted");
}

AMethod *HTTPActionDEL::create()
{
	return (new HTTPActionDEL());
}
