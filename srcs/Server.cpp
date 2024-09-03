#include "Server.hpp"

Server::Server(const ServerConfig &config) : _config(config)
{
    std::ostringstream tmp;
    tmp << _config.getPort();

    std::cout << "Launching '" << highlight(_config.getName(), false) << "' on " << "\e]8;;" << "http://" << _config.getHost() << ":" << tmp.str() << "\e\\" << GREEN << _config.getHost() + ":" + tmp.str() << WHITE << "\e]8;;\e\\" << std::endl;
}

int Server::prepareServer(void)
{
    _serverFd = socket(AF_INET, SOCK_STREAM, 0);

    if (_serverFd == -1)
        throw ServerException("Cannot create ServerSocket");

    int reuseOpt = true;

    if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &reuseOpt, sizeof(reuseOpt)) < 0)
    {
        close(_serverFd);
        throw ServerException("Cannot set SO_REUSE mode");
    }

    struct sockaddr_in addr;

    if (!inet_aton(_config.getHost().c_str(), &addr.sin_addr))
    {
        close(_serverFd);
        throw ServerException("Invalid Host : " + _config.getHost());
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(_config.getPort());

    if (bind(_serverFd, (struct sockaddr *)&addr, sizeof(addr)))
    {
        close(_serverFd);
        throw ServerException("Cannot bind ServerSocket");
    }

    if (listen(_serverFd, 5))
    {
        close(_serverFd);
        throw ServerException("Cannot start ServerSocket");
    }

    return _serverFd;
}

int Server::getFd(void) const
{
    return (this->_serverFd);
}

void Server::addClient(int fd, Client *client)
{
    _clients.insert(std::make_pair(fd, client));
}

void Server::removeClient(std::map<int, Client *>::iterator pos)
{
    _clients.erase(pos);
}

std::map<int, Client *> &Server::getClients(void)
{
    return (this->_clients);
}

void Server::stopClients(void) const
{
    std::map<int, Client *>::const_iterator it = _clients.begin();

    for (; it != _clients.end(); it++)
    {
        close((*it).first);
        delete ((*it).second);
    }
}

const ServerConfig &Server::getConfig(void) const
{
    return (this->_config);
}

Server::~Server()
{
}

Server::ServerException::ServerException(const std::string &message) : _message(std::string(RED) + "Server Error" + std::string(YELLOW) + " - " + message)
{
}

const char *Server::ServerException::what() const throw()
{
    return (_message.c_str());
}

std::ostream &operator<<(std::ostream &os, const Server &server)
{
    os << server.getConfig().getHost() << ":" << server.getConfig().getPort();
    return (os);
}
