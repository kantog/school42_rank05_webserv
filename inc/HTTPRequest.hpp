
#pragma once 

#include <string>
#include <vector>

class	HTTPRequest {
	private:
		std::string _method;
		std::string _requestTarget;
		std::string _hostURL;
		std::vector <std::string> _headers;
		std::string _body;

		void		setMethod(const std::string &input);
		void		setRequestTarget(const std::string &input);
		void		setHostURL(const std::string &input);
		void		setHeaders(const std::vector <std::string> &input);
		void		setBody(const std::string &input);

	public:
		HTTPRequest();
		HTTPRequest(std::string method,
			std::string requestTarget,
			std::string header,
			const std::vector <std::string> &headers,
			std::string body);
		HTTPRequest(const HTTPRequest &other);
		HTTPRequest &operator=(const HTTPRequest &other);
		~HTTPRequest();

		const std::string			&getMethod() const;
		const std::string			&getRequestTarget() const;
		const std::string			&getHostURL() const;
		std::vector <std::string>	&getHeaders() const;
		const std::string			&getBody() const;

		void parseRequest(std::string rawRequest);
};
