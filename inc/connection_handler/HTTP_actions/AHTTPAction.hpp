
#pragma once 

#include <string>
#include <vector>
#include "../HTTPRequest.hpp"
#include "../HTTPResponse.hpp"
// #include "../ServerConfig.hpp"

class	AHTTPAction {
	private:
		HTTPResponse &_response;
		HTTPRequest &_request;
		// ServerConfig & _serverConfig;//TODO: toevoegen als ServerConfig class klaar is

		virtual void _implementMethod() = 0;//implements actions specific to the child classes and fills HTTPResponse
											//
	public:
		AHTTPAction(HTTPResponse & _response, 
				HTTPRequest & _request);
			// ServerConfig & _serverConfig);
		AHTTPAction(const AHTTPAction &other);
		AHTTPAction &operator=(const AHTTPAction &other);
		~AHTTPAction();

		void sessionManager();//TODO: wat moet dit doen?
		void generateErrorResponse(int errorCode);//sets status code and body and returns
		void run();
};
