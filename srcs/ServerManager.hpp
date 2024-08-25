#pragma once

#include <iostream>
#include <string>
#include <exception>

#include "config/ConfigParser.hpp"
#include "Server.hpp"

class ServerManager
{
public:
    ServerManager();
    ~ServerManager();
    void run(const std::string &path);
    void stopServers(void);

private:
    std::vector<ServerConfig> _configs;
    std::vector<Server *> _servers;
};