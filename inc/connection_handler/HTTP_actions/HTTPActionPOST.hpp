
#pragma once

#include "AMethod.hpp"

#include <string>

class ServerConfig;
class HTTPRequest;
class HTTPResponse;

class HTTPActionPOST : public AMethod
{
private:
	void post(HTTPRequest &request,
			  HTTPResponse &response,
			  const ServerConfig &serverConfig);

	void downloadMultiPart(HTTPRequest &request,
			  HTTPResponse &response,
			  const ServerConfig &serverConfig);
	
	void downloadFile(HTTPRequest &request,
			  HTTPResponse &response,
			  const ServerConfig &serverConfig, const std::string &body);

public:
	HTTPActionPOST();
	~HTTPActionPOST();

	static AMethod *create();
	void implementMethod(HTTPRequest &request,
						 HTTPResponse &response,
						 const ServerConfig &serverConfig);
};
