
#pragma once 

#include "MethodRegistry.hpp"
#include "../HTTPResponse.hpp"
#include "Cgi.hpp"

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
		HTTPResponse _response;
		const ServerConfig & _serverConfig;
		MethodRegistry _methodRegistry;

		Cgi *_cgi;

	public:
		HTTPAction(HTTPRequest & _request, 
				const ServerConfig  &_serverConfig);
		HTTPAction(const HTTPAction &other);
		~HTTPAction();

		void run();

		bool isCgiRunning();
		Cgi *getCgi();
		std::string getFullResponseString();
		std::string getFullErrorResponseString(int statusCode);
		std::string getFullCgiResponseString();
};
