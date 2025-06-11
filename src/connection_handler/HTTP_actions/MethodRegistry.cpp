
#include "../../../inc/connection_handler/HTTP_actions/MethodRegistry.hpp"
#include "../../../inc/connection_handler/HTTP_actions/HTTPActionPOST.hpp"
#include "../../../inc/connection_handler/HTTP_actions/HTTPActionGET.hpp"
#include "../../../inc/connection_handler/HTTP_actions/HTTPActionDEL.hpp"

MethodRegistry::MethodRegistry() 
{
	_methods["POST"] = &HTTPActionPOST::create;
	_methods["GET"] = &HTTPActionGET::create;
	_methods["DEL"] = &HTTPActionDEL::create;
}

MethodRegistry::~MethodRegistry()
{
		
}

AMethod *MethodRegistry::createMethodInstance(const std::string &methodInput) //find relevant method and return new instance of it
{
	return(_methods.find(methodInput)->second());
}
