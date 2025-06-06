
#include "../inc/config_classes/MyConfig.hpp"
#include "../inc/HTTPServer.hpp"

int main(int argc, char **argv)
{
    try
    {
        if (argc > 1)
            MyConfig::get(argv[1]);
        else
        {
            std::cout << "Using default config file" << std::endl;
            MyConfig::get("default.conf");
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }

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
