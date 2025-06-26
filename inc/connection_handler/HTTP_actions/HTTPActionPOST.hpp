
#pragma once 

#include "AMethod.hpp"

class ServerConfig;
class HTTPRequest;
class HTTPResponse;

class	HTTPActionPOST : public AMethod {
	private:
		void post(HTTPRequest &request,
						HTTPResponse & response, 
						const ServerConfig &serverConfig);

		std::string getUploadPath(const ServerConfig &serverConfig) const;

	public:
		HTTPActionPOST();
		~HTTPActionPOST();

		static AMethod *create();
		void implementMethod(HTTPRequest &request,
						HTTPResponse & response, 
						const ServerConfig &serverConfig);
};
