#pragma once

#include <string>
#include <vector>
#include <exception>
#include <fstream>

#include "ServerConfig.hpp"

class ConfigParser
{
public:
    ConfigParser(const std::string& path);
    const std::vector<ServerConfig>& getConfigs() const;

private:
    std::vector<ServerConfig> _configs;
    void parse(const std::string& path);

    // Exception class
    class ParsingException : public std::exception
    {
    public:
        ParsingException(const std::string& message);
        virtual ~ParsingException() throw() {};
        virtual const char* what() const throw();
    private:
        const std::string _message;
    };
};