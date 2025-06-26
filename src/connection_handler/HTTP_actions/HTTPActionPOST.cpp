#include "../../../inc/connection_handler/HTTP_actions/HTTPActionPOST.hpp"
#include "../../../inc/config_classes/ServerConfig.hpp"
#include "../../../inc/connection_handler/HTTPRequest.hpp"
#include "../../../inc/connection_handler/HTTPResponse.hpp"
#include "../../../inc/Defines.hpp"

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
		response.setStatusCode(HTTP_METHOD_NALLOWED);
		return ;
	}

	std::ofstream fileToPost(serverConfig.
			getFullFilesystemPath(request.
				getRequestTarget()).c_str());
	if (!fileToPost.is_open())
	{
		if (errno == EISDIR
				&& (request.getHeader("Content-Type").find_first_of("multipart/form-data")
					!= request.getHeader("content-type").npos))
		{
			
			std::string newFile = serverConfig.getCurentRoute().uploadPath;
			fileToPost.open(newFile.c_str());
		}
		if (!fileToPost.is_open())
		{
			response.setStatusCode(HTTP_SERVER_ERROR);
			return ;
		}
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
	response.setBody("Congratulations, you successfully uploaded a file!");
}

AMethod *HTTPActionPOST::create()
{
	return (new HTTPActionPOST());
}
