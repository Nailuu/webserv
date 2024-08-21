#include "Server.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <exception>

Server::Server(const GlobalConfig& global, const ServerConfig &local): _global(global), _local(local)
{
    std::cout << "Launching Server on port " << local.getPort() << std::endl;
}

void Server::prepareServer(void)
{
    _serverFd = _maxFd = socket(AF_INET, SOCK_STREAM, 0);

    if (_serverFd == -1)
        throw ServerException("Cannot create ServerSocket");
    
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
    while (it != _clients.end())
        FD_SET((*it++).first, &_readFds);
}

void Server::waitForUpdate(void)
{
    if (select(_maxFd + 1, &_readFds, NULL, NULL, NULL))
        throw new ServerException("Error while recovering datas with select");
    
    if (FD_ISSET(_stopPipe[0], &_readFds))
        throw new ServerException("Server was stopped");
}

void Server::newClientCheck(void)
{
    // Has a new client arrived?
    if (!FD_ISSET(_serverFd, &_readFds))
        return ;
    
    struct sockaddr_in infos;
    socklen_t infos_len;

    int client = accept(_serverFd, (struct sockaddr *)&infos, &infos_len);

    if (client == -1)
        throw ServerException("Cannot accept client");
    
    if (client > _maxFd)
        _maxFd = client;
    
    _clients.insert(std::map<int, struct sockaddr_in>::value_type(client, infos));
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
        throw ServerException("Server is not started");
}

void *run(void *ptr)
{
    Server *server = (Server *)ptr;

    while (true)
    {
        server->prepareFds();
        
        try {
            server->waitForUpdate();
            server->newClientCheck();
        } catch (std::exception &e) {
            std::cerr << "[" << server << "] " << e.what() << std::endl;
            break ;
        }

        server->readCheck();
    }

    server->closeAll();
    return (NULL);
}

Server::~Server()
{
}

Server::ServerException::ServerException(const std::string &message) : _message("Server error - " + message) {}

const char *Server::ServerException::what() const throw()
{
    return (_message.c_str());
}

std::ofstream &operator<<(std::ofstream &os, const Server &server)
{
    os << server._local.getHost() << ":" << server._local.getPort();
    return (os);
}
