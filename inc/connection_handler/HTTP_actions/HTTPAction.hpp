
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
		// SessionManager &_sessionManager //TODO: 

		Cgi *_cgi;

	public:
		HTTPAction(HTTPRequest & _request, 
				const ServerConfig  &_serverConfig);
		HTTPAction(const HTTPAction &other);
		~HTTPAction();

		// void sessionManager();//TODO: wat moet dit doen? 
		// void generateErrorResponse(int errorCode);//sets status code and body and returns
		void run();

		bool isCgiRunning();
		Cgi *getCgi();
		std::string getFullResponseString();
		std::string getFullCgiResponseString();
};
