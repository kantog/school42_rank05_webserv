
#include "../../../inc/connection_handler/HTTP_actions/MethodRegistry.hpp"
#include "../../../inc/connection_handler/HTTP_actions/HTTPActionPOST.hpp"
#include "../../../inc/connection_handler/HTTP_actions/HTTPActionGET.hpp"
#include "../../../inc/connection_handler/HTTP_actions/HTTPActionDEL.hpp"
#include <stdexcept>
#include <map>

MethodRegistry::MethodRegistry() 
{
	_methods["POST"] = &HTTPActionPOST::create;
	_methods["GET"] = &HTTPActionGET::create;
	_methods["DELETE"] = &HTTPActionDEL::create;
}

MethodRegistry::~MethodRegistry()
{
		
}

AMethod *MethodRegistry::createMethodInstance(const std::string &methodInput)
{
	std::map<std::string, f>::iterator it = _methods.find(methodInput);
	if (it == _methods.end())
		throw std::runtime_error("Error something went wrong while creating "
				"new " + methodInput + " method class");
	return((*it).second());
}
