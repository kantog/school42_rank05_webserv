
#pragma once 

#include <string>
#include "AHTTPAction.hpp"

class	HTTPActionGET : public AHTTPAction {
	private:

	public:
		HTTPActionGET();
		HTTPActionGET(const HTTPActionGET &other);
		HTTPActionGET &operator=(const HTTPActionGET &other);
		~HTTPActionGET();

		virtual void runCGI();//runt een script en zet output in _response.body:
		void fetchFile();

		//GCI: CGI is executed and
};
