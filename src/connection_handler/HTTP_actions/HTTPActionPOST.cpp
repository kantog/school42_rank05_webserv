#include "../../../inc/connection_handler/HTTP_actions/HTTPActionPOST.hpp"
#include "../../../inc/config_classes/ServerConfig.hpp"
#include "../../../inc/connection_handler/HTTPRequest.hpp"
#include "../../../inc/connection_handler/HTTPResponse.hpp"
#include "../../../inc/ErrorCodes.hpp"

#include <cstring>
#include <fstream>
#include <iostream>


HTTPActionPOST::HTTPActionPOST()
{ }

HTTPActionPOST::~HTTPActionPOST()
{ }

void HTTPActionPOST::implementMethod(HTTPRequest &request,
						HTTPResponse & response, 
						const ServerConfig &serverConfig)
{
	if (!serverConfig.getCurentRoute().uploadAllowed)
	{
		response.setStatusCode(HTTP_CREATED);
		return ;
	}

	std::ofstream fileToPost(serverConfig.getFullFilesystemPath(request.getRequestTarget()).c_str());// die . gwn in ReqTarget zetten?
	if (!fileToPost.is_open())
	{
		std::cerr << "Error: couldn't open file" << std::endl;
		response.setStatusCode(HTTP_SERVER_ERROR);
		return ;
	}

	fileToPost << request.getBody();
	fileToPost.close();

	if (fileToPost.fail())
	{
		std::cerr << "Error: couldn't write to file" << std::endl;
		response.setStatusCode(HTTP_SERVER_ERROR);
		return ;
	}

	response.setStatusCode(HTTP_CREATED);
}

AMethod *HTTPActionPOST::create()
{
	return (new HTTPActionPOST());
}
