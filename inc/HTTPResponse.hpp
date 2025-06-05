

#pragma once

#include <string>
#include <map>

class HTTPResponse
{	
	public:
		HTTPResponse();
		~HTTPResponse();

		void reset();
		void buildResponse();

		const std::string &getResponseString() const;

		void setStatusCode(int code);
		void setStatusMessage(const std::string &message);
		void setHeader(const std::string &key, const std::string &value);
		void setBody(const std::string &body, const std::string &contentType = "text/plain");
		void setBodyFromFile(const std::string &filePath, const std::string &contentType = "text/html");
		void setRedirect(const std::string &location, int code = 302);

		void setBodySize(); // public?

	private:

		int _statusCode;
		std::string _statusText;
		std::map<std::string, std::string> _headers;
		std::string _body;

		std::string _responseString;

		void _setStatusMessage(int code);

};
