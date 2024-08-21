#include "ServerManager.hpp"

ServerManager::ServerManager(const std::string& path)
{
    std::cout << "Launching ServerManager with configuration file: \"" << path << "\"" << std::endl;

    // Create config object and validate config file
    try {
        this->_config = GlobalConfig(path);
    } catch(std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    // Run ServerManager with config file
    this->run();
}

ServerManager::~ServerManager()
{
    std::cout << "Closing ServerManager..." << std::endl;
}

void ServerManager::run()
{
    const std::vector<ServerConfig> configs = this->_config.getServers();
    std::vector<ServerConfig>::const_iterator it = configs.begin();

    while (it != configs.end())
        Server(_config, (*it++));
}
