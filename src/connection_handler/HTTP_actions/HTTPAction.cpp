
#include "../../../inc/connection_handler/HTTP_actions/AMethod.hpp"
#include "../../../inc/connection_handler/HTTP_actions/HTTPAction.hpp"
#include "../../../inc/connection_handler/HTTPRequest.hpp"
#include "../../../inc/connection_handler/HTTPResponse.hpp"
#include "ServerConfig.hpp"
#include "Cgi.hpp"

HTTPAction::HTTPAction(HTTPRequest & request,
						const ServerConfig &serverConfig):
	_request(request), 
	_response(), 
	_serverConfig(serverConfig),
	_cgi(NULL)
{ 
	_response.reset();
}

HTTPAction::HTTPAction(const HTTPAction &other):
	_request(other._request),
	_response(other._response),
	_serverConfig(other._serverConfig),
	_cgi(other._cgi)
{ }

HTTPAction::~HTTPAction()
{ }

// void generateErrorResponse(int errorCode) //sets status code and body and return
// { }

bool HTTPAction::isCgiRunning()
{
	return (_cgi != NULL);
}

Cgi *HTTPAction::getCgi()
{
	return _cgi;
}

std::string HTTPAction::getFullCgiResponseString() 
{
	this->_response.buildCgiPage(_cgi->getBody());
	return (_response.getResponseString());
}

std::string HTTPAction::getFullResponseString() 
{
	return (_response.getResponseString());
}

void HTTPAction::run()
{
	_response.setHeader("Set-Cookie", _request.getHeader("Cookie")); // TODO: test cookies
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
		// this->_response.buildCgiPage(_cgi->getBody());//test
		return;
	}
	else
	{
		AMethod *HTTPMethod 
			= _methodRegistry.createMethodInstance(_request.getMethod());
		HTTPMethod->implementMethod(_request, _response, _serverConfig);
		delete HTTPMethod;
	}

	int errorCode = _response.getStatusCode();
	if (errorCode < 200 || errorCode > 226)
		_response.buildErrorPage(errorCode, 
				_serverConfig.getErrorPagePath(errorCode));
	//TODO: hier return page optie aan toevoegen?
	else
		_response.buildResponse();
	// TODO: flag if not yet build?
}
