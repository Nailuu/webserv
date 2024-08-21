#pragma once

#include <string>
#include <vector>
#include <iostream>

#include "global.hpp"

class ServerConfig
{
public:
    ServerConfig();
    ServerConfig(const ServerConfig& sc);
    ~ServerConfig();
    ServerConfig& operator=(const ServerConfig& sc);
    int getPort() const;

private:
    int _port;
    std::string _host;
    std::vector<HTTP_METHOD> _accepted_http_methods;
};