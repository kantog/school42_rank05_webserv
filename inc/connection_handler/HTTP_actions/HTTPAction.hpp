
#pragma once 

#include "MethodRegistry.hpp"
#include "Cgi.hpp"
#include <exception>
#include <string>
#include <vector>

class HTTPRequest;
class HTTPResponse;
class ServerConfig;
class SessionManager;
class ErrorManager;


class	HTTPAction
{
	private:
		HTTPAction(); 

		HTTPRequest &_request;
		HTTPResponse &_response;
		const ServerConfig & _serverConfig;
		MethodRegistry _methodRegistry;

		Cgi *_cgi;

	public:
		HTTPAction(HTTPRequest & _request, 
				HTTPResponse  &_response,
				const ServerConfig  &_serverConfig);
		HTTPAction(const HTTPAction &other);
		~HTTPAction();

		void run();

		bool isCgiRunning();
		Cgi *getCgi();
};
