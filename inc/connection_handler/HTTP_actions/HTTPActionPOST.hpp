
#pragma once 

#include <string>
#include "AMethod.hpp"

class ServerConfig;
class HTTPRequest;
class HTTPResponse;

class	HTTPActionPOST : public AMethod {
	private:
		// void _runCGI();//runt een script en zet output in _response.body:
		void post(HTTPRequest &request,
						HTTPResponse & response, 
						const ServerConfig &serverConfig);

	public:
		HTTPActionPOST();
		~HTTPActionPOST();

		static AMethod *create();
		void implementMethod(HTTPRequest &request,
						HTTPResponse & response, 
						const ServerConfig &serverConfig);
};
