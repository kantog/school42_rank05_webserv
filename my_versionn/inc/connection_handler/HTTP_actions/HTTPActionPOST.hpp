
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

	void downloadMulti(HTTPRequest &request,
			  HTTPResponse &response,
			  const ServerConfig &serverConfig);

public:
	HTTPActionPOST();
	~HTTPActionPOST();

	static AMethod *create();
	void implementMethod(HTTPRequest &request,
						 HTTPResponse &response,
						 const ServerConfig &serverConfig);
};
