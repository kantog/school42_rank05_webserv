
#include "../../../inc/connection_handler/HTTP_actions/AMethod.hpp"
#include "../../../inc/connection_handler/HTTP_actions/HTTPAction.hpp"
// #include "../../../inc/connection_handler/HTTP_actions/HTTPActionDEL.hpp"
// #include "../../../inc/connection_handler/HTTP_actions/HTTPActionPOST.hpp"
// #include "../../../inc/connection_handler/HTTP_actions/HTTPActionGET.hpp"
#include "../../../inc/connection_handler/HTTPRequest.hpp"
#include "../../../inc/connection_handler/HTTPResponse.hpp"
#include "ServerConfig.hpp"
#include "Cgi.hpp"
#include <iostream>

HTTPAction::HTTPAction(HTTPRequest & request,
						HTTPResponse & response, 
						const ServerConfig &serverConfig):
	_request(request), 
	_response(response), 
	_serverConfig(serverConfig),
	_cgi(NULL)
{ }

HTTPAction::HTTPAction(const HTTPAction &other):
	_request(other._request),
	_response(other._response),
	_serverConfig(other._serverConfig),
	_cgi(other._cgi)
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

// BASILL
bool HTTPAction::isCgiRunning()
{
	return (_cgi != NULL);
}

Cgi *HTTPAction::getCgi()
{
	return _cgi;
}

void HTTPAction::run()
{
	
	if (!_serverConfig.isAllowedMethod(_request.getMethod()))
		_response.setStatusCode(405);
	else if (_serverConfig.isAllowedCgi(_request.getRequestTarget()))
	{
		Cgi *cgi = new Cgi(_request, _serverConfig);
		cgi->startCgi();
		int code = cgi->getStatusCode();
		if (code != 200)
		{
			_response.buildErrorPage(code, _serverConfig.getErrorPagePath(code));
			delete cgi;
		}
		else
			_cgi = cgi;
		return;
	}
	else
	{
		//specific method logic
		AMethod *HTTPMethod 
			= _methodRegistry.createMethodInstance(_request.getMethod());
		HTTPMethod->implementMethod(_request, _response, _serverConfig);
		delete HTTPMethod;
	}

	//internal HTTPAction logic
	int errorCode = _response.getStatusCode();
	// std::cout << errorCode << std::endl;//test
	// std::cout << _serverConfig.getErrorPagePath(errorCode) << std::endl;//test
	if (errorCode < 200 || errorCode > 226)
		_response.buildErrorPage(errorCode, 
				_serverConfig.getErrorPagePath(errorCode));
	//TODO: hier autoindex optie aan toevoegen?
	//TODO: hier return page optie aan toevoegen?
	else
		_response.buildResponse();
	// TODO: flag if not yet build?
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
