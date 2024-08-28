#include <iostream>

#include "global.hpp"
#include "ServerManager.hpp"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << "usage: ./webserv <configuration-file>" << std::endl;
        return (0);
    }

    try
    {
        // Launch ServerManager that will parse configuration file and manager servers
        ServerManager manager;
        manager.init(argv[1]);
        manager.run();
    }
    catch (std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return (1);
    }

    return (0);
}
