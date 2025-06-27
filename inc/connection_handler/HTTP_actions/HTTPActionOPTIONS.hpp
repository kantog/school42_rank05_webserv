
#pragma once 

#include "AMethod.hpp"

class ServerConfig;
class HTTPRequest;
class HTTPResponse;

class	HTTPActionOPTIONS : public AMethod {
	private:

	public:
		HTTPActionOPTIONS();
		~HTTPActionOPTIONS();

		static AMethod *create();
		void implementMethod(HTTPRequest &request,
						HTTPResponse & response, 
						const ServerConfig &serverConfig);
};
