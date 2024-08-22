#pragma once

#include <pthread.h>
#include <map>
#include <iostream>
#include <sys/select.h>

#include "ServerConfig.hpp"

class Server
{
public:
    Server(const ServerConfig &config);
    ~Server();
    friend std::ostream &operator<<(std::ostream &os, const Server &server);
    void prepareServer(void);
    void prepareFds(void);
    bool waitForUpdate(void);
    bool newClientCheck(void);
    std::map<std::string, std::string> getRequestInfos(const std::string &buffer);
    void readCheck(void);
    void closeAll(void);
    void stop(void);

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