
#pragma once

#include <string>
#include <map>

class HTTPResponse
{	
	private:
		int _statusCode;
		std::string _statusText;
		std::map<std::string, std::string> _headers;
		std::string _body;

		std::string _responseString;

		void _setStatusMessage(int code);
		std::string _createDirString(const std::string &directoryPath,
				const std::string &appendString = "");
		
		void _setCustomErrorBody(const std::string &filePath);

	public:
		HTTPResponse();
		~HTTPResponse();

		void reset();
		void buildResponse();
		void buildErrorPage(int code, const std::string &filePath);
		void buildReturnPage(int code, const std::string &filePath);
		void buildDirectoryPage(const std::string &filePath);//TODO
		void buildCgiPage(const std::string &cgiString);

		const std::string &getResponseString() const;
		const int &getStatusCode() const;

		void setStatusCode(int code);
		void setStatusMessage(const std::string &message);
		void setHeader(const std::string &key, const std::string &value);
		void setHeaders(const std::string &headers);
		void setBody(const std::string &body, const std::string &contentType = "text/html");
		void setBodyFromFile(const std::string &filePath, const std::string &contentType = "text/html");
		void setRedirect(const std::string &location, int code = 302);
		
		void setContentType(void);
		void setBodySize(); // public?
};
