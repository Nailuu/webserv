#pragma once

#include <iostream>
#include <string>
#include <exception>

#include "GlobalConfig.hpp"
#include "ServerConfig.hpp"
#include "Server.hpp"

class ServerManager
{
public:
    ServerManager(const std::string& path);
    ~ServerManager();

private:
    GlobalConfig _config;
    void run();
};