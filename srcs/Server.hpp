#pragma once

#include "GlobalConfig.hpp"
#include "ServerConfig.hpp"

class Server
{
public:
    Server(const GlobalConfig& config, int port);
    ~Server();

private:
    const GlobalConfig& _global;
    const ServerConfig& _local;
};