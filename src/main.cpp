
#include "../inc/config_classes/MyConfig.hpp"
#include "../inc/HTTPServer.hpp"

#include <signal.h>
#include <iostream>

HTTPServer *globalServerPtr = NULL;

void handleSigint(int signum)
{
    if (globalServerPtr)
        globalServerPtr->stop(signum);
}

int main(int argc, char **argv)
{
    try
    {
        // TODO: als eror trow, lijst prineten met wat wel mag
        if (argc > 1)
        {
            MyConfig::get(argv[1]); // TODO: check if file exists ? .conf?
            std::cout << "Using config file " << argv[1] << std::endl;
        }
        else
        {
            std::cout << "Using default config file" << std::endl;
            MyConfig::get("default/default.conf");
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

        globalServerPtr = &server;
        signal(SIGINT, handleSigint); // CTRL+C
        server.start();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
