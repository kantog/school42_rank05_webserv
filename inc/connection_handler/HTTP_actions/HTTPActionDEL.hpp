
#pragma once 

#include "AMethod.hpp"

class ServerConfig;
class HTTPRequest;
class HTTPResponse;

class	HTTPActionDEL : public AMethod {
	private:

	public:
		HTTPActionDEL();
		~HTTPActionDEL();

		static AMethod *create();
		void implementMethod(HTTPRequest &request,
						HTTPResponse & response, 
						const ServerConfig &serverConfig);
};
