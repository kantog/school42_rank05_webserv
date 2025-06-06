
#include "../inc/connection_handler/HTTP_actions/AHTTPAction.hpp"

AHTTPAction::AHTTPAction(HTTPResponse & response, 
						HTTPRequest & request):
						// ServerConfig &serverConfig):
	_response(response), _request(request)//, _serverConfig(serverConfig)
{

}

AHTTPAction::AHTTPAction(const AHTTPAction &other):
	_response(other._response),
	_request(other._request)
	// _serverConfig(other._serverConfig)
{

}

AHTTPAction &AHTTPAction::operator=(const AHTTPAction &other)
{
	_response = other._response; 
	_request = other._request;
	// _serverConfig = other._serverConfig;
	return (*this);
}

AHTTPAction::~AHTTPAction()
{

}

void AHTTPAction::sessionManager()
{

}

void generateErrorResponse(int errorCode) //sets status code and body and return
{
		
}

void run()
{
		//e.g.
		// void run ()
		// {
		// 	if (_response.error)
		// 		generateErrorResponse()//
		// 	else if (checkPermissions)
		// 		generateErrorResponse()//
		// 	else
		// 	{
		// 		//dingen die alle child classes moeten doen
		// 		_implementMethod();
		// 	}
		// }
}
