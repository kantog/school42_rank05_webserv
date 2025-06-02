

# include "../inc/MyConfig.hpp"
// # include "../inc/HTTPServer.hpp"

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    MyConfig::get("default.conf");

   	MyConfig::get();
    // HTTPServer server;
    return 0;
}
