#pragma once

#include <iostream>
#include <string>

#include "Config.hpp"

class Server
{
public:
    Server(const std::string& config);
    ~Server();

private:
    Config _config;
    void run();
};