
#pragma once 

#include <string>
#include "AHTTPAction.hpp"

class	HTTPActionDEL : public AHTTPAction {
	private:

	public:
		HTTPActionDEL();
		HTTPActionDEL(const HTTPActionDEL &other);
		HTTPActionDEL &operator=(const HTTPActionDEL &other);
		~HTTPActionDEL();

		void deleteFile();


		//GCI: if server allows it, CGI can also be executed
};
