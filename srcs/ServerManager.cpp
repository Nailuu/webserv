#include "ServerManager.hpp"

ServerManager::ServerManager() : _maxFd(0) {}

ServerManager::~ServerManager()
{
    std::cout << GREY << "Closing ServerManager..." << std::endl;
}

void ServerManager::stopServers(void)
{
    std::vector<Server *>::iterator it = _servers.begin();

    for (; it != _servers.end(); it++)
    {
        Server *server = *it;
        server->stopClients();
        close(server->getFd());
        delete server;
    }
}

void ServerManager::init(const std::string &path)
{
    std::cout << GREY << "Launching ServerManager with configuration file: '" << BLUE << path << GREY << "'" << WHITE << std::endl;

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
            int fd = server->prepareServer();
            if (fd > _maxFd)
            {
                _maxFd = fd;
            }
            _servers.push_back(server);
        }
        catch (std::exception &e)
        {
            delete server;
            throw;
        }

        std::vector<ServerConfig>::const_iterator tmp = it;
        if (++tmp != this->_configs.end())
            std::cout << GREY << "\n------------------------------------------\n\n"
                      << WHITE << std::endl;
    }
}

void ServerManager::run(void)
{
    while (true)
    {
        prepareFds();

        if (!waitForUpdate() || !newClientCheck())
            break;

        readCheck();
        writeCheck();
    }

    stopServers();
}

void ServerManager::prepareFds(void)
{
    // Reset FDS
    FD_ZERO(&_readFds);
    FD_ZERO(&_writeFds);

    // Put stdin
    FD_SET(STDIN_FILENO, &_readFds);

    // Put serverFds
    std::vector<Server *>::iterator servIt = _servers.begin();

    for (; servIt != _servers.end(); servIt++)
    {
        Server *server = (*servIt);
        FD_SET(server->getFd(), &_readFds);

        std::map<int, Client *>::iterator it = server->getClients().begin();

        // Put all clients fd
        for (; it != server->getClients().end(); it++)
        {
            FD_SET((*it).first, &_readFds);
            if (!(*it).second->isReceiving())
            {
                FD_SET((*it).first, &_writeFds);
            }
        }
    }
}

bool ServerManager::waitForUpdate(void)
{
    if (select(_maxFd + 1, &_readFds, &_writeFds, NULL, NULL) < 0)
        return (false);

    if (FD_ISSET(STDIN_FILENO, &_readFds))
    {
        std::string line;
        std::getline(std::cin, line);

        if (line.compare("stop") == 0)
        {
            return (false);
        }
    }

    return (true);
}

bool ServerManager::newClientCheck(void)
{
    // Has a new client arrived?
    std::vector<Server *>::iterator it = _servers.begin();

    for (; it != _servers.end(); it++)
    {
        Server *server = (*it);
        int _serverFd = server->getFd();

        if (!FD_ISSET(_serverFd, &_readFds))
            continue;

        struct sockaddr_in infos;
        socklen_t infos_len = sizeof(infos);

        int fd_client = accept(_serverFd, (struct sockaddr *)&infos, &infos_len);

        if (fd_client == -1)
            return (false);

        if (fd_client > _maxFd)
            _maxFd = fd_client;

        server->addClient(fd_client, new Client(fd_client));
    }

    return (true);
}

void ServerManager::readCheck(void)
{
    std::vector<Server *>::iterator servIt = _servers.begin();

    for (; servIt != _servers.end(); servIt++)
    {
        Server *server = (*servIt);
        FD_SET(server->getFd(), &_readFds);

        std::map<int, Client *>::iterator it = server->getClients().begin();

        std::vector<std::map<int, Client *>::iterator> removed;

        // iterate all clients and check if we have received something
        for (; it != server->getClients().end(); it++)
        {
            std::pair<int, Client *> client = *it;

            if (!FD_ISSET(client.first, &_readFds))
                continue;

            try
            {
                if (!client.second->onReceive())
                {
                    client.second->onFinishReceiving(server->getConfig());
                }
                continue;
            }
            catch (std::exception &e)
            {
            }

            removed.push_back(it);
            client.second->onStop();
            delete (client.second);
        }

        std::vector<std::map<int, Client *>::iterator>::iterator it2 = removed.begin();

        for (; it2 != removed.end(); it2++)
        {
            server->removeClient(*it2);
        }
    }
}

void ServerManager::writeCheck(void)
{
    std::vector<Server *>::iterator servIt = _servers.begin();

    for (; servIt != _servers.end(); servIt++)
    {
        Server *server = (*servIt);
        FD_SET(server->getFd(), &_readFds);

        std::map<int, Client *>::iterator it = server->getClients().begin();

        std::vector<std::map<int, Client *>::iterator> removed;

        // iterate all clients and check if we have received something
        for (; it != server->getClients().end(); it++)
        {
            std::pair<int, Client *> client = *it;

            if (!FD_ISSET(client.first, &_writeFds))
                continue;

            try
            {
                if (client.second->onSend())
                {
                    continue;
                }
            }
            catch (std::exception &e)
            {
            }

            removed.push_back(it);
            client.second->onStop();
            delete (client.second);
        }

        std::vector<std::map<int, Client *>::iterator>::iterator it2 = removed.begin();

        for (; it2 != removed.end(); it2++)
        {
            server->removeClient(*it2);
        }
    }
}
