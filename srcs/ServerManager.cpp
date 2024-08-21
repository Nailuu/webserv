#include "ServerManager.hpp"

ServerManager::ServerManager(const std::string& path)
{
    std::cout << "Launching ServerManager with configuration file: \"" << path << "\"" << std::endl;

    // Create config object and validate config file
    try {
        this->_config = GlobalConfig(path);
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    // Run ServerManager with config file
    this->run();
}

ServerManager::~ServerManager()
{
    std::cout << "Closing ServerManager..." << std::endl;
}

void ServerManager::stopServers(void)
{
    std::vector<Server*>::iterator it = _servers.begin();

    for (; it != _servers.end(); it++)
    {
        Server *server = *it;
        try {
            std::cout << "Calling stop() on server." << std::endl;
            server->stop();
            std::cout << "Finished stop() on server." << std::endl;
        } catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
        }
        std::cout << "Deleting server." << std::endl;
        delete server;
    }
}

void ServerManager::run()
{
    const std::vector<ServerConfig> configs = this->_config.getServers();
    std::vector<ServerConfig>::const_iterator it = configs.begin();

    for (; it != configs.end(); it++)
    {
        Server *server = new Server(_config, (*it));

        try {
            server->prepareServer();
            _servers.push_back(server);
        } catch (std::exception &e) {
            std::cout << "j'ai catch ici bg" << std::endl;
            std::cerr << "[" << server << "] " << e.what() << std::endl;
            delete server;
        }
    }
}
