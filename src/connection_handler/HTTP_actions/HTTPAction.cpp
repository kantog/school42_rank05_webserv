
#include "../../../inc/connection_handler/HTTP_actions/AMethod.hpp"
#include "../../../inc/connection_handler/HTTP_actions/HTTPActionDEL.hpp"
#include "../../../inc/connection_handler/HTTP_actions/HTTPAction.hpp"
#include "../../../inc/connection_handler/HTTP_actions/HTTPActionPOST.hpp"
#include "../../../inc/connection_handler/HTTP_actions/HTTPActionGET.hpp"
#include "../../../inc/connection_handler/HTTPRequest.hpp"
#include "../../../inc/connection_handler/HTTPResponse.hpp"
#include "../../../inc/config_classes/ServerConfig.hpp"
#include <iostream>

HTTPAction::HTTPAction(HTTPRequest & request,
						HTTPResponse & response, 
						const ServerConfig &serverConfig):
	_request(request), 
	_response(response), 
	_serverConfig(serverConfig)
{ }

HTTPAction::HTTPAction(const HTTPAction &other):
	_request(other._request),
	_response(other._response),
	_serverConfig(other._serverConfig)
{ }

HTTPAction::~HTTPAction()
{ }

// void HTTPAction::sessionManager()
// {
//
// }

// void generateErrorResponse(int errorCode) //sets status code and body and return
// {
//
// }

void HTTPAction::run()
{
	//internal HTTPAction logic
	
	
	//specific method logic
	AMethod *HTTPMethod 
		= _methodRegistry.createMethodInstance(_request.getMethod());
	HTTPMethod->implementMethod(_request, _response, _serverConfig);

	int errorCode = _response.getStatusCode();
	std::cout << errorCode << std::endl;//test
	std::cout << _serverConfig.getErrorPagePath(errorCode) << std::endl;//test
	if (errorCode < 200 || errorCode > 226)
		_response.buildErrorPage(errorCode, 
				_serverConfig.getErrorPagePath(errorCode));

	delete HTTPMethod;

}

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
