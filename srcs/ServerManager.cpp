#include "ServerManager.hpp"

ServerManager::ServerManager()
{
}

ServerManager::~ServerManager()
{
    std::cout << "Closing ServerManager..." << std::endl;
}

void ServerManager::stopServers(void)
{
    std::vector<Server *>::iterator it = _servers.begin();

    for (; it != _servers.end(); it++)
    {
        Server *server = *it;
        try {
            server->stop();
        } catch (std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
        delete server;
    }
}

void ServerManager::run(const std::string &path)
{
    std::cout << "Launching ServerManager with configuration file: \"" << path << "\"" << std::endl;

    // Create config object and parse configuration file
    try
    {
        ConfigParser c;
        c.parse(path);

        this->_configs = c.getConfigs();
    }
    catch (std::exception &e)
    {
        throw;
    }

    std::vector<ServerConfig>::const_iterator it = this->_configs.begin();

    for (; it != this->_configs.end(); it++)
    {
        Server *server = new Server(*it);

        try
        {
            server->prepareServer();
            _servers.push_back(server);
        }
        catch (std::exception &e)
        {
            delete server;
            throw;
        }
    }
}