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
        ServerManager manager(argv[1]);

        // std::string input;

        // while (true)
        // {
        //     std::getline(std::cin, input);

        //     if (std::cin.eof() || !input.compare("stop"))
        //         break;
        // }

        // manager.stopServers();
    }
    catch (std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return (1);
    }

    return (0);
}
