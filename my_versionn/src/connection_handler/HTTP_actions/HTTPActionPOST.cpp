#include "../../../inc/connection_handler/HTTP_actions/HTTPActionPOST.hpp"
#include "../../../inc/config_classes/ServerConfig.hpp"
#include "../../../inc/connection_handler/HTTPRequest.hpp"
#include "../../../inc/connection_handler/HTTPResponse.hpp"
#include "../../../inc/Defines.hpp"

#include <cstring>
#include <fstream>
#include <iostream>
#include <ctime>

HTTPActionPOST::HTTPActionPOST()
{
}

HTTPActionPOST::~HTTPActionPOST()
{
}

void HTTPActionPOST::downloadMulti(HTTPRequest &request,
								   HTTPResponse &response,
								   const ServerConfig &serverConfig)
{
	std::ofstream fileToPost(
		serverConfig.getUploadPath(request.getRequestTarget()).c_str(),
		std::ios::out | std::ios::binary | std::ios::app);
	if (!fileToPost.is_open())
	{
		response.setStatusCode(HTTP_SERVER_ERROR);
		return;
	}

	std::time_t now = std::time(NULL);
    char timeBuffer[100];
    std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
	
	fileToPost << "[" << timeBuffer << "]\n";
	fileToPost << request.getBody() << "\n\n";
	
	fileToPost.close();

	if (fileToPost.fail())
	{
		std::cerr << "Error: couldn't write to file" << std::endl;
		response.setStatusCode(HTTP_SERVER_ERROR);
		return;
	}

	response.setStatusCode(HTTP_CREATED);
	response.setBody("Congratulations, you successfully uploaded a file!");
}

void HTTPActionPOST::implementMethod(HTTPRequest &request,
									 HTTPResponse &response,
									 const ServerConfig &serverConfig)
{
	if (!serverConfig.getCurentRoute().uploadAllowed)
	{
		response.setStatusCode(HTTP_METHOD_NALLOWED);
		return;
	}

	if (request.getHeader("Content-Type").find_first_of("multipart/form-data") != request.getHeader("content-type").npos)
	{
		downloadMulti(request, response, serverConfig);
		return;
	}
}

AMethod *HTTPActionPOST::create()
{
	return (new HTTPActionPOST());
}
