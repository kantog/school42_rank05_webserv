

# include "../inc/MyConfig.hpp"
# include "../inc/HTTPServer.hpp"

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    // MyConfig::get("default.conf");
	// std::string test ="jdjd";

   	// MyConfig::get();
	
	try 
	{
    HTTPServer server;
	server.init();
	server.start();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
    return 0;
}
