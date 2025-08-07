
#pragma once 

class HTTPRequest;
class HTTPResponse;
class ServerConfig;

class	AMethod {
	public:
		virtual void implementMethod(HTTPRequest &request,
						HTTPResponse & response, 
						const ServerConfig &serverConfig) = 0;
		virtual ~AMethod() {};
};
