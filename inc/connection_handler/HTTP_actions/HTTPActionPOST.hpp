
#pragma once 

#include <string>
#include "AHTTPAction.hpp"

class	HTTPActionPOST : public AHTTPAction {
	private:

	public:
		HTTPActionPOST();
		HTTPActionPOST(const HTTPActionPOST &other);
		HTTPActionPOST &operator=(const HTTPActionPOST &other);
		~HTTPActionPOST();
		
		virtual void runCGI();//runt een script en zet output in _response.body:
		void post();
};
