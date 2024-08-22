#pragma once

#include <string>
#include <vector>
#include <exception>
#include <fstream>
#include <sstream>
#include <map>

#include "ServerConfig.hpp"
#include "JSON.hpp"

class ConfigParser
{
public:
    ConfigParser(const std::string& path);
    const std::vector<ServerConfig>& getConfigs() const;

private:
    std::string _json;
    std::vector<ServerConfig> _configs;
    void parse(const std::string& path);
    void sanitize();

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