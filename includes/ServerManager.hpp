#pragma once

#include <iostream>
#include <string>
#include <exception>
#include <map>

#include "ConfigParser.hpp"
#include "Server.hpp"

class ServerManager
{
public:
    ServerManager();
    ~ServerManager();
    void init(const std::string &path);
    void run();
    void stopServers(void);
    void clear(void);

private:
    std::vector<ServerConfig> _configs;
    std::vector<Server *> _servers;
    CookiesManager _cookiesManager;
    int _maxFd;
    fd_set _readFds;
    fd_set _writeFds;
    fd_set _errorFds;
    void prepareFds(void);
    bool waitForUpdate(void);
    bool newClientCheck(void);
    void readCheck(void);
    void writeCheck(void);
};