#include "Server.hpp"

Server::Server(const std::string& config)
{
    std::cout << "Launching HTTP server with configuration file: \"" << config << "\"" << std::endl;

    // Create config object and validate config file
    this->_config = Config(config);

    if (!this->_config.valid()) {
        return ;
    }

    // Run server with config file
    this->run();
}

Server::~Server()
{
    std::cout << "Closing server..." << std::endl;
}

void Server::run()
{

}
