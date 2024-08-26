#include "Server.hpp"

Server::Server(const ServerConfig &config) : _config(config)
{
    std::cout << "Launching Server on port " << _config.getPort() << std::endl;
    std::cout << "Host of first server " << _config.getHost() << std::endl;
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
            break;

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

    std::vector<std::map<int, struct sockaddr_in>::iterator> removed;

    // iterate all clients and check if we have received something
    for (; it != _clients.end(); it++)
    {
        std::pair<int, struct sockaddr_in> client = *it;

        if (!FD_ISSET(client.first, &_readFds))
            continue;

        bzero(_buffer, sizeof(_buffer));

        if (read(client.first, _buffer, sizeof(_buffer)) <= 0)
        {
            close(client.first);
            removed.push_back(it);
            continue;
        }

        try
        {
            Request req = Request::fromString(_buffer);

            // Handle request route
            const std::string& route = req.getRoute();
            if (!this->_config.routeExists(route))
            {
                // TODO: Error 404
            }

            // Get config for this route
            const Route& config = this->_config.getRoute(route);

            HTTP_METHOD method = req.getMethod();
            if (!config.isHTTPMethodAuthorized(method))
            {
                // TODO: Error 405
            }

            // Output for demonstration
            std::cout << "Request Method: " << getStringFromHttpMethod(method) << std::endl;
            std::cout << "Request Route: " << route << std::endl;
            std::cout << "HTTP Version: " << req.getHttpVersion() << std::endl;
            std::cout << "Host: " << req.getHost() << std::endl;

            std::map<std::string, std::string>::const_iterator it = req.getFields().begin();

            for (; it != req.getFields().end(); it++)
            {
                std::pair<std::string, std::string> info = *it;

                std::cout << info.first << ": " << info.second << std::endl;
            }

            Response res("HTTP/1.1", NotFound);
            res.setContentFile("data/404.html");
            res.addField("Connection", "close");

            const std::string res_str = res.build();

            // TODO: Non-blocking write
            write(client.first, res_str.c_str(), res_str.length());
        }
        catch (std::exception &e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    std::vector<std::map<int, struct sockaddr_in>::iterator>::iterator it2 = removed.begin();

    for (; it2 != removed.end(); it2++)
    {
        _clients.erase(*it2);
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

const ServerConfig &Server::getConfig() const
{
    return (this->_config);
}

Server::~Server()
{
}

Server::ServerException::ServerException(const std::string &message) : _message("Server Error - " + message)
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
