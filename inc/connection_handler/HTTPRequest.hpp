
#pragma once

#include <map>
#include <string>

class HTTPRequest
{
	private:
		std::string _method;
		std::string _requestTarget;
		std::string _hostURL;
		std::map<std::string, std::string> _headers;
		std::string _body;
		//boolean: isReady

		void _setMethod(const std::string &input);
		void _setRequestTarget(const std::string &input);
		void _setHostURL(const std::string &input);
		void _setHeaders(const std::map<std::string, std::string> &input);
		void _setBody(const std::string &input);

		void _fillHeaders(std::string line);
		void _printRequest() const;

	public:
		HTTPRequest();
		HTTPRequest(std::string method,
				std::string requestTarget,
				std::string hostURL,
				const std::map<std::string, std::string> &headers,
				std::string body);
		HTTPRequest(const HTTPRequest &other);
		HTTPRequest &operator=(const HTTPRequest &other);
		~HTTPRequest();

		const std::string &getMethod() const;
		const std::string &getRequestTarget() const;
		const std::string &getHostURL() const;
		const std::map<std::string, std::string> &getHeaders() const;
		const std::string &getHeader(const std::string &key) const;
		const std::string &getBody() const;


		void reset();
		bool hasCloseHeader() const;
		void parseRequest(std::string rawRequest);
};
