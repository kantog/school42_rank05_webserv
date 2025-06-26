#include "../../../inc/connection_handler//HTTP_actions/HTTPActionDEL.hpp"
#include "../../../inc/config_classes/ServerConfig.hpp"
#include "../../../inc/connection_handler/HTTPRequest.hpp"
#include "../../../inc/connection_handler/HTTPResponse.hpp"
#include "../../../inc/Defines.hpp"

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
		response.setStatusCode(HTTP_BADREQ);
		return;
	}
	errno = 0;
	int exitCode = std::remove(serverConfig.getFullFilesystemPath(request.getRequestTarget()).c_str());
	if (exitCode != 0)
	{
		switch (errno)
		{
			case  EACCES: 	
				response.setStatusCode(HTTP_FORBIDDEN);
				return;
			case ENOENT:	
				response.setStatusCode(HTTP_NOTFOUND);
				return;
			case EISDIR:	
				response.setStatusCode(HTTP_CONFLICT);
				return;
			default: 
				response.setStatusCode(HTTP_SERVER_ERROR);
				return;
		}
	}
	response.setBody("Success: " + request.getRequestTarget() + " deleted");
}

AMethod *HTTPActionDEL::create()
{
	return (new HTTPActionDEL());
}
