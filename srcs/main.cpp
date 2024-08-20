#include <iostream>
#include "Server.hpp"

int main(int argc, char **argv)
{
	if (argc != 2)
    {
        std::cout << "usage: ./webserv <configuration-file>" << std::endl;
        return (0);
    }

	Server s(argv[1]);

	return (0);
}
