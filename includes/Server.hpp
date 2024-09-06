#pragma once

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <exception>
#include <string.h>
#include <sstream>
#include <pthread.h>
#include <map>
#include <iostream>
#include <sys/select.h>

#include "global.hpp"
#include "ServerConfig.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Client.hpp"

class Server
{
public:
    Server(const ServerConfig &config);
    ~Server();
    int prepareServer(void);
    int getFd(void) const;
    void addClient(int fd, Client *client);
    void removeClient(std::map<int, Client*>::iterator pos);
    std::map<int, Client*> &getClients(void);
    void stopClients(void) const;
    const ServerConfig& getConfig(void) const;

private:
    const ServerConfig& _config;
    int _serverFd;
    std::map<int, Client*> _clients;
    
    class ServerException : public std::exception
    {
    public:
        ServerException(const std::string& message);
        virtual ~ServerException() throw() {};
        virtual const char* what() const throw();
    private:
        const std::string _message;
    };
};

std::ostream &operator<<(std::ostream &os, const Server &server);