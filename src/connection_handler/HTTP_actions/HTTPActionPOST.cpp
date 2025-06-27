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

void HTTPActionPOST::downloadFile(HTTPRequest &request,
								  HTTPResponse &response,
								  const ServerConfig &serverConfig, const std::string &body)
{
	std::ofstream fileToPost(
		serverConfig.getUploadPath(request.getRequestTarget()).c_str(),
		std::ios::out | std::ios::binary | std::ios::app);
	if (!fileToPost.is_open())
	{
		response.setStatusCode(HTTP_SERVER_ERROR);
		return;
	}

	std::cout << "[INFO]: appending data in file: " << serverConfig.getUploadPath(request.getRequestTarget()) << std::endl;
	std::time_t now = std::time(NULL);
	char timeBuffer[100];
	std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

	fileToPost << "[" << timeBuffer << "]\n";
	fileToPost << body << "\n\n";

	fileToPost.close();

	if (fileToPost.fail())
	{
		std::cerr << "Error: couldn't write to file" << std::endl;
		response.setStatusCode(HTTP_SERVER_ERROR);
		return;
	}

	response.setStatusCode(HTTP_CREATED);
	response.setHeader("Access-Control-Allow-Origin", "*");
	response.setHeader("Access-Control-Allow-Methods", "DELETE, OPTIONS, POST, GET");
	response.setHeader("Access-Control-Allow-Headers", "Content-Type");
}

std::string getBoundary(std::string &contentType)
{
	std::string boundaryPrefix = "boundary=";
	std::size_t boundaryPos = contentType.find(boundaryPrefix);
	if (boundaryPos == std::string::npos)
		return "";
	return "--" + contentType.substr(boundaryPos + boundaryPrefix.length());
}

std::string getFilename(std::string &headers)
{
	std::string filename;
	std::size_t namePos = headers.find("filename=\"");
	if (namePos != std::string::npos)
	{
		std::size_t start = namePos + 10;
		std::size_t end = headers.find("\"", start);
		return headers.substr(start, end - start);
	}
	return "";
}

void saveFile(std::string &filePath, std::string &content, HTTPResponse &response)
{
	std::ofstream outFile(filePath.c_str(), std::ios::binary);
	if (!outFile.is_open())
	{
		std::cerr << "Error opening file: " << filePath << std::endl;
		response.setStatusCode(HTTP_SERVER_ERROR);
		return;
	}
	outFile << content;
	outFile.close();
	std::cout << "[INFO]: Saved file: " << filePath << std::endl;
}

void HTTPActionPOST::downloadMultiPart(HTTPRequest &request,
									   HTTPResponse &response,
									   const ServerConfig &serverConfig)
{
	std::string contentType = request.getHeader("Content-Type");
	std::string body = request.getBody();
	std::string boundary = getBoundary(contentType);
	if (boundary == "")
	{
		response.setStatusCode(HTTP_BADREQ);
		return;
	}

	std::stringstream ss(body);
	std::string line;
	bool inPart = false;
	bool headersComplete = false;
	std::string headers = "";
	std::string content = "";
	
	while (std::getline(ss, line))
	{
		// Remove \r
		if (!line.empty() && line[line.length() - 1] == '\r')
			line.erase(line.length() - 1);
			
		if (line == boundary || line == boundary + "--")
		{
			if (inPart && headersComplete)
			{
				std::string filename = getFilename(headers);
				
				if (!filename.empty())
				{
					std::string filePath = serverConfig.getUploadPath(filename);
					saveFile(filePath, content, response);
				}
				else 
					downloadFile(request, response, serverConfig, content);
			}
			if (line == boundary + "--")
				break;
			inPart = true;
			headersComplete = false;
			headers = "";
			content = "";
		}
		else if (inPart)
		{
			if (!headersComplete)
			{
				if (line.empty())
					headersComplete = true;
				else
					headers += line + "\n";
			}
			else
			{
				if (!content.empty())
					content += "\n";
				content += line;
			}
		}
	}

	response.setStatusCode(HTTP_CREATED);
	response.setBody("Multipart upload succesvol!");
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

	std::string contentType = request.getHeader("Content-Type");
	if (contentType.find("boundary=") != std::string::npos)
	{
		downloadMultiPart(request, response, serverConfig);
		return;
	}
	downloadFile(request, response, serverConfig, request.getBody());
}

AMethod *HTTPActionPOST::create()
{
	return (new HTTPActionPOST());
}
