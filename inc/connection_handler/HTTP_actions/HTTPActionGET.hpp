
#pragma once 

#include "AMethod.hpp"

class ServerConfig;
class HTTPRequest;
class HTTPResponse;

class	HTTPActionGET : public AMethod {
	private:
		void _runCGI();//runt een script en zet output in _response.body:
		void _fetchFile(HTTPRequest &request,
				HTTPResponse & response, 
				const ServerConfig &serverConfig);
		void _checkForIndexFiles(HTTPResponse & response, 
				const ServerConfig &serverConfig);

	public:
		HTTPActionGET();
		~HTTPActionGET();

		static AMethod *create();
		void implementMethod(HTTPRequest &request,
						HTTPResponse & response, 
						const ServerConfig &serverConfig);
};
