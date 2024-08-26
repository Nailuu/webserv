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
#include "config/ServerConfig.hpp"
#include "request/Request.hpp"
#include "request/Response.hpp"
#include "enum/HttpMethod.hpp"

class Server
{
public:
    Server(const ServerConfig &config);
    ~Server();
    void prepareServer(void);
    void prepareFds(void);
    bool waitForUpdate(void);
    bool newClientCheck(void);
    void readCheck(void);
    void closeAll(void);
    void stop(void);
    const ServerConfig& getConfig() const;

private:
    const ServerConfig& _config;
    char _buffer[4096];
    int _serverFd, _maxFd;
    int _stopPipe[2];
    fd_set _readFds;
    std::map<int, struct sockaddr_in> _clients;
    pthread_t _thread;
    
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