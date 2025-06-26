
#include "../../../inc/connection_handler/HTTPRequest.hpp"
#include "../../../inc/connection_handler/HTTPResponse.hpp"
#include "../../../inc/config_classes/ServerConfig.hpp"
#include "../../../inc/connection_handler/HTTP_actions/HTTPActionGET.hpp"
#include "../../../inc/Defines.hpp"

#include <unistd.h>
#include <vector>
#include <sys/stat.h>

HTTPActionGET::HTTPActionGET()
{
}

HTTPActionGET::~HTTPActionGET()
{ }

bool HTTPActionGET::_checkForIndexFiles(HTTPResponse & response, 
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
		if (response.getStatusCode() == HTTP_OK)
			return (true);
	}
	return (false);
}

void HTTPActionGET::_fetchFile(HTTPRequest &request,
							   HTTPResponse &response,
							   const ServerConfig &serverConfig)
{
	std::string filePath = serverConfig.getFullFilesystemPath(request.getRequestTarget());


	struct stat fileInfo; 
	if (stat(filePath.c_str(), &fileInfo) == -1)
	{
		response.setStatusCode(HTTP_NOTFOUND);
		return;
	}

	if (!S_ISDIR(fileInfo.st_mode))
		response.setBodyFromFile(filePath);
	else
	{
		if (!(serverConfig.getCurentRoute().defaultFiles.empty()))
		{
			if (this->_checkForIndexFiles(response, serverConfig))
				return ;
		}
		if (serverConfig.getCurentRoute().isDirectoryListing)
		{
			response.buildDirectoryPage(serverConfig.getFullFilesystemPath(
						serverConfig.getCurentRoute().path));
		}
		else
		{
			response.setStatusCode(HTTP_NOTFOUND);
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
