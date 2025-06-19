
#include "../../../inc/connection_handler/HTTPRequest.hpp"
#include "../../../inc/connection_handler/HTTPResponse.hpp"
#include "../../../inc/config_classes/ServerConfig.hpp"
#include "../../../inc/connection_handler/HTTP_actions/HTTPActionGET.hpp"

#include <stdexcept>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <sys/stat.h>

HTTPActionGET::HTTPActionGET()
{
}

HTTPActionGET::~HTTPActionGET()
{ }

void HTTPActionGET::_checkForIndexFiles(HTTPResponse & response, 
		const ServerConfig &serverConfig)
{
	for (std::vector<std::string>::const_iterator it 
			= serverConfig.getCurentRoute().defaultFiles.begin();
			it != serverConfig.getCurentRoute().defaultFiles.end(); it++)
	{
		response.reset();
		response //index file not found 
			.setBodyFromFile(serverConfig
					.getFullPath("/" + *it));
		if (response.getStatusCode() == 200)
			break ;
	}
}

void HTTPActionGET::_fetchFile(HTTPRequest &request,
							   HTTPResponse &response,
							   const ServerConfig &serverConfig)
{
	std::string filePath = serverConfig.getFullPath(request.getRequestTarget());

	struct stat fileInfo; 
	if (stat(filePath.c_str(), &fileInfo) == -1)
		throw std::runtime_error("Error calling stat()");

	// Checking if file exists, else checking index files or auto-index
	if (!S_ISDIR(fileInfo.st_mode))
		response.setBodyFromFile(filePath);
	else
	{
		if (!(serverConfig.getCurentRoute().defaultFiles.empty()))
			this->_checkForIndexFiles(response, serverConfig);
		if (serverConfig.getCurentRoute().isDirectoryListing)
		{
			response.buildDirectoryPage(serverConfig.getFullPath(
						serverConfig.getCurentRoute().path));
		}
	}
}

void HTTPActionGET::implementMethod(HTTPRequest &request,
		HTTPResponse & response, 
		const ServerConfig &serverConfig)
{	
	this->_fetchFile(request, response, serverConfig);
}

AMethod *HTTPActionGET::create()
{
	return (new HTTPActionGET());
}
