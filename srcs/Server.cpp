#include "Server.hpp"

Server::Server(const GlobalConfig& config, int port): _global(config), _local(_global.getServerConfig(port))
{
    std::cout << "Launching Server on port " << port << std::endl;
}

Server::~Server()
{

}
