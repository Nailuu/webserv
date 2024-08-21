#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <exception>
#include <vector>

#include "ServerConfig.hpp"

class GlobalConfig
{
public:
    GlobalConfig();
    GlobalConfig(const std::string& path);
    GlobalConfig(const GlobalConfig& gc);
    ~GlobalConfig();
    GlobalConfig& operator=(const GlobalConfig& gc);
    const ServerConfig& getServerConfig(int port) const;

private:
    std::vector<ServerConfig> _servers;

    // Exception class
    class InvalidConfigException : public std::exception
    {
    public:
        InvalidConfigException(const std::string& message);
        virtual ~InvalidConfigException() throw() {};
        virtual const char* what() const throw();

    private:
        std::string _message;
    };
};