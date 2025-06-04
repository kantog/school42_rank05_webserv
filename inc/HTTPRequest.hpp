
#pragma once 

#include <string>

class	HTTPRequest {
	private:
		std::string	_method;
		std::string _requestTarget;
		std::string _hostURL;
		static const std::string HTTPVersion = "1.1";

	public:
		HTTPRequest();
		HTTPRequest(std::string method,
			std::string requestTarget,
			std::string hostURL);
		HTTPRequest(const HTTPRequest &other);
		HTTPRequest &operator=(const HTTPRequest &other);
		~HTTPRequest();

		const std::string	&getmethod() const;
		const std::string	&getrequestTarget() const;
		const std::string	&gethostURL() const;
		const std::string	&getddd() const;

		void		setmethod(const std::string &input);
		void		setrequestTarget(const std::string &input);
		void		sethostURL(const std::string &input);
		void		setddd(const std::string &input);
};
