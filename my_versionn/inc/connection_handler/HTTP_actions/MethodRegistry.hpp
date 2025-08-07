
#pragma once 

#include <string>
#include <map>

class AMethod;
class HTTPActionPOST;
class HTTPActionGET;
class HTTPActionDEL;

class	MethodRegistry {
	private:
		typedef AMethod*(*f)();
		std::map <std::string, f> _methods;
 
	public:
		MethodRegistry();
		~MethodRegistry();

		AMethod *createMethodInstance(const std::string &methodInput);
};
