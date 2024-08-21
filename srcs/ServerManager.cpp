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

}
