
#pragma once 

#include <string>

class	HTTPHandler {
	private:
		HTTPRequest	HTTPRequest;
		HTTPResponse HTTPResponse;

	public:
		HTTPHandler();
		HTTPHandler(std::string aaa,
			std::string bbb,
			std::string ccc,
			std::string ddd);
		HTTPHandler(const HTTPHandler &other);
		HTTPHandler &operator=(const HTTPHandler &other);
		~HTTPHandler();
};


