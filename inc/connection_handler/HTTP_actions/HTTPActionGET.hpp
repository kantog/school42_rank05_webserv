
#pragma once 

#include "AMethod.hpp"

class ServerConfig;
class HTTPRequest;
class HTTPResponse;

class	HTTPActionGET : public AMethod {
	private:
		void _fetchFile(HTTPRequest &request,
				HTTPResponse & response, 
				const ServerConfig &serverConfig);
		bool _checkForIndexFiles(HTTPResponse & response, 
				const ServerConfig &serverConfig);

	public:
		HTTPActionGET();
		~HTTPActionGET();

		static AMethod *create();
		void implementMethod(HTTPRequest &request,
						HTTPResponse & response, 
						const ServerConfig &serverConfig);
};
