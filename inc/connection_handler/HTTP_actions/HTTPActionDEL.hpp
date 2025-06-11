
#pragma once 

#include <string>
#include "AMethod.hpp"

class ServerConfig;
class HTTPRequest;
class HTTPResponse;

class	HTTPActionDEL : public AMethod {
	private:
		// void _deleteFile();

	public:
		HTTPActionDEL();
		~HTTPActionDEL();

		static AMethod *create();
		void implementMethod(HTTPRequest &request,
						HTTPResponse & response, 
						const ServerConfig &serverConfig);

		//GCI: if server allows it, CGI can also be executed
};
