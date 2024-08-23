#pragma once

#include <iostream>
#include <string>
#include <exception>

#include "ConfigParser.hpp"
#include "Server.hpp"

class ServerManager
{
public:
    ServerManager(const std::string &path);
    ~ServerManager();
    void stopServers(void);

private:
    std::vector<ServerConfig> _configs;
    std::vector<Server *> _servers;
    void run();
};