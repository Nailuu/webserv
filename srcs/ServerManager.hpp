#pragma once

#include <iostream>
#include <string>
#include <exception>
#include <map>

#include "config/ConfigParser.hpp"
#include "Server.hpp"

class ServerManager
{
public:
    ServerManager();
    ~ServerManager();
    void init(const std::string &path);
    void run();
    void stopServers(void);

private:
    std::vector<ServerConfig> _configs;
    std::vector<Server *> _servers;
    int _maxFd;
    fd_set _readFds;
    fd_set _writeFds;
    void prepareFds(void);
    bool waitForUpdate(void);
    bool newClientCheck(void);
    void readCheck(void);
    void writeCheck(void);
};