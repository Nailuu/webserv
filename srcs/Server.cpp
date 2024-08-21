#include "Server.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <exception>
#include <string.h>

Server::Server(const GlobalConfig& global, const ServerConfig &local): _global(global), _local(local)
{
    std::cout << "Launching Server on port " << local.getPort() << std::endl;
    std::cout << "Host of first server " << _global.getServers().at(0).getHost() << std::endl;
}

void *run(void *ptr)
{
    Server *server = (Server *)ptr;

    while (true)
    {
        std::cout << "début boucle !" << std::endl;

        server->prepareFds();

        std::cout << "fds préparés !" << std::endl;
        
        if (!server->waitForUpdate() || !server->newClientCheck())
            break ;
        
        std::cout << "tout s'est bien passé !!" << std::endl;

        server->readCheck();
    }

    std::cout << "[" << *server << "]" << " terminé !" << std::endl;
    server->closeAll();
    return (NULL);
}

void Server::prepareServer(void)
{
    _serverFd = _maxFd = socket(AF_INET, SOCK_STREAM, 0);

    if (_serverFd == -1)
        throw ServerException("Cannot create ServerSocket");
    
    int reuseOpt = true;

    if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &reuseOpt, sizeof(reuseOpt)) < 0)
    {
        close(_serverFd);
        throw ServerException("Cannot set SO_REUSE mode");
    }
    
    struct sockaddr_in addr;

    if (!inet_aton(_local.getHost().c_str(), &addr.sin_addr))
    {
        close(_serverFd);
        throw ServerException("Invalid Host : " + _local.getHost());
    }
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_local.getPort());

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

    if (pipe(_stopPipe))
    {
        close(_serverFd);
        throw ServerException("Cannot create stop pipe");
    }

    if (_stopPipe[0] > _maxFd)
        _maxFd = _stopPipe[0];
    
    if (pthread_create(&_thread, NULL, run, this))
    {
        close(_stopPipe[0]);
        close(_stopPipe[1]);
        close(_serverFd);
        throw ServerException("Cannot start Server Thread");
    }
}

void Server::prepareFds(void)
{
    // Reset FDS
    FD_ZERO(&_readFds);

    // Put stopPipe
    FD_SET(_stopPipe[0], &_readFds);

    // Put serverFd
    FD_SET(_serverFd, &_readFds);

    std::map<int, struct sockaddr_in>::iterator it = _clients.begin();

    // Put all clients fd
    for (; it != _clients.end(); it++)
        FD_SET((*it).first, &_readFds);
}

bool Server::waitForUpdate(void)
{
    if (select(_maxFd + 1, &_readFds, NULL, NULL, NULL) < 0)
        return false;
    
    return (!FD_ISSET(_stopPipe[0], &_readFds));
}

bool Server::newClientCheck(void)
{
    // Has a new client arrived?
    if (!FD_ISSET(_serverFd, &_readFds))
        return (true);
    
    std::cout << "j'ai un client !" << std::endl;
    
    struct sockaddr_in infos;
    socklen_t infos_len = sizeof(infos);

    int client = accept(_serverFd, (struct sockaddr *)&infos, &infos_len);

    if (client == -1)
        return (false);
    
    if (client > _maxFd)
        _maxFd = client;
    
    _clients.insert(std::map<int, struct sockaddr_in>::value_type(client, infos));
    return (true);
}

void Server::readCheck(void)
{
    std::map<int, struct sockaddr_in>::iterator it = _clients.begin();

    // iterate all clients and check if we have received something
    for (; it != _clients.end(); it++)
    {
        std::pair<int, struct sockaddr_in> client = *it;

        if (!FD_ISSET(client.first, &_readFds))
            continue ;
        
        std::cout << "je lis qlq chose" << std::endl;

        bzero(_buffer, sizeof(_buffer));
            
        if (read(client.first, _buffer, sizeof(_buffer)) <= 0)
        {
            close(client.first);
            _clients.erase(it);
            continue ;
        }

        std::cout << client.first << ": " << _buffer << std::endl;
    }
}

void Server::closeAll(void)
{
    std::map<int, struct sockaddr_in>::iterator it = _clients.begin();

    while (it != _clients.end())
        close((*it++).first);
    
    close(_stopPipe[0]);
    close(_stopPipe[1]);
    close(_serverFd);
}

void Server::stop(void)
{
    if (write(_stopPipe[1], "", 1) != 1)
        throw ServerException("Server is not running");
    if (pthread_join(_thread, NULL))
        throw ServerException("Cannot join Server Thread");
}

Server::~Server()
{
}

Server::ServerException::ServerException(const std::string &message) : _message("Server error - " + message) {}

const char *Server::ServerException::what() const throw()
{
    return (_message.c_str());
}

std::ostream &operator<<(std::ostream &os, const Server &server)
{
    os << server._local.getHost() << ":" << server._local.getPort();
    return (os);
}
