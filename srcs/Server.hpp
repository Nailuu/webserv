#pragma once

#include "GlobalConfig.hpp"
#include "ServerConfig.hpp"
#include <pthread.h>
#include <map>
#include <iostream>

class Server
{
    public:
        Server(const GlobalConfig& global, const ServerConfig &local);
        ~Server();
        friend std::ofstream &operator<<(std::ofstream &os, const Server &server);
        void prepareServer(void);
        void prepareFds(void);
        void waitForUpdate(void);
        void newClientCheck(void);
        void readCheck(void);
        void closeAll(void);
        void stop(void);

    private:
        const GlobalConfig& _global;
        const ServerConfig& _local;
        char _buffer[4096];
        int _serverFd, _maxFd;
        int _stopPipe[2];
        fd_set _readFds;
        std::map<int, struct sockaddr_in> _clients;
        pthread_t _thread;
    
    class ServerException: public std::exception
    {
        public:
            ServerException(const std::string& message);
            virtual ~ServerException() throw() {};
            virtual const char* what() const throw();
        private:
            const std::string _message;
    };
};