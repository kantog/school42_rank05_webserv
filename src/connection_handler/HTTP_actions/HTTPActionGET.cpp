
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
					.getFullFilesystemPath(*it));
		if (response.getStatusCode() == 200)
			break ;
	}
}

void HTTPActionGET::_fetchFile(HTTPRequest &request,
							   HTTPResponse &response,
							   const ServerConfig &serverConfig)
{
	std::string filePath = serverConfig.getFullFilesystemPath(request.getRequestTarget());


	/*
	struct stat sb;
    if (stat(_path.c_str(), &sb) == 0)
    {
        if (S_ISREG(sb.st_mode) && (sb.st_mode & S_IXUSR))
            _statusCode = 200; 
        else
            _statusCode = 403; // script not executable
    }
    else
        _statusCode = 404; // script not found
	*/
	struct stat fileInfo; 
	if (stat(filePath.c_str(), &fileInfo) == -1)  //TODO ?  if (access(filePath.c_str(), F_OK) != 0) 
		{
			response.setStatusCode(404);
			return;
		}
		// throw std::runtime_error("Error calling stat()");

	// Checking if file exists, else checking index files or auto-index
	if (!S_ISDIR(fileInfo.st_mode))
		response.setBodyFromFile(filePath);
	else
	{
		if (!(serverConfig.getCurentRoute().defaultFiles.empty()))
			this->_checkForIndexFiles(response, serverConfig);
		if (serverConfig.getCurentRoute().isDirectoryListing)
		{
			response.buildDirectoryPage(serverConfig.getFullFilesystemPath(
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
