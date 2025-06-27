#include "../../../inc/connection_handler/HTTP_actions/HTTPActionPOST.hpp"
#include "../../../inc/config_classes/ServerConfig.hpp"
#include "../../../inc/connection_handler/HTTPRequest.hpp"
#include "../../../inc/connection_handler/HTTPResponse.hpp"
#include "../../../inc/Defines.hpp"

#include <cstring>
#include <fstream>
#include <iostream>
#include <ctime>
#include <vector>

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
	fileToPost.write(body.c_str(), body.length());
	fileToPost << "\n\n";

	fileToPost.close();

	if (fileToPost.fail())
	{
		std::cerr << "Error: couldn't write to file" << std::endl;
		response.setStatusCode(HTTP_SERVER_ERROR);
		return;
	}
	response.setBody("success", "text/plain");
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

void saveFile(std::string &filePath, const std::vector<char> &content, HTTPResponse &response)
{
	std::ofstream outFile(filePath.c_str(), std::ios::binary);
	if (!outFile.is_open())
	{
		std::cerr << "Error opening file: " << filePath << std::endl;
		response.setStatusCode(HTTP_SERVER_ERROR);
		return;
	}

	outFile.write(content.data(), content.size());
	outFile.close();

	if (outFile.fail())
	{
		std::cerr << "Error writing to file: " << filePath << std::endl;
		response.setStatusCode(HTTP_SERVER_ERROR);
		return;
	}

	std::cout << "[INFO]: Saved file: " << filePath << " (" << content.size() << " bytes)" << std::endl;
}

static std::string getHeaders(std::string &body, size_t &pos)
{
	std::string headers;

	size_t headerEnd = body.find("\r\n\r\n", pos);
	if (headerEnd == std::string::npos)
	{
		headerEnd = body.find("\n\n", pos);
		if (headerEnd == std::string::npos)
			return "";
		headers = body.substr(pos, headerEnd - pos);
		pos = headerEnd + 2;
	}
	else
	{
		headers = body.substr(pos, headerEnd - pos);
		pos = headerEnd + 4;
	}

	return headers;
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

	size_t pos = 0;
	std::string boundaryDelim = boundary;
	std::string endBoundary = boundary + "--";

	while (pos < body.length())
	{
		size_t boundaryPos = body.find(boundaryDelim, pos);
		if (boundaryPos == std::string::npos)
		{
			response.setStatusCode(HTTP_BADREQ);
			return;
		}

		pos = boundaryPos + boundaryDelim.length();
		if ((pos < body.length() && body[pos] == '\r') || (pos < body.length() && body[pos] == '\n'))
			pos++;
		if (body.substr(boundaryPos, endBoundary.length()) == endBoundary)
			break; // done

		std::string headers = getHeaders(body, pos);
		if (headers == "")
		{
			response.setStatusCode(HTTP_BADREQ);
			return;
		}

		size_t nextBoundaryPos = body.find(boundaryDelim, pos);
		if (nextBoundaryPos == std::string::npos)
		{
			response.setStatusCode(HTTP_BADREQ);
			return;
		}
		
		size_t contentLength = nextBoundaryPos - pos;
		if (contentLength >= 2 && body[nextBoundaryPos - 2] == '\r' && body[nextBoundaryPos - 1] == '\n')
			contentLength -= 2;
		else if (contentLength >= 1 && body[nextBoundaryPos - 1] == '\n')
			contentLength -= 1;

		std::vector<char> content(body.begin() + pos, body.begin() + pos + contentLength);
		std::string filename = getFilename(headers);

		if (!filename.empty())
		{
			std::string filePath = serverConfig.getUploadPath(filename);
			saveFile(filePath, content, response);
		}
		else
		{
			std::string textContent(content.begin(), content.end());
			downloadFile(request, response, serverConfig, textContent);
		}
		pos = nextBoundaryPos;
	}

	if (response.getStatusCode() == HTTP_CREATED)
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

	response.setStatusCode(HTTP_CREATED);
	response.setHeader("Access-Control-Allow-Origin", "*");
	response.setHeader("Access-Control-Allow-Methods", "DELETE, POST, GET");
	response.setHeader("Access-Control-Allow-Headers", "Content-Type");

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