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

	ServerManager manager(argv[1]);

	return (0);
}
