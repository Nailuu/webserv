#pragma once

#include <string>
#include <vector>
#include <exception>
#include <fstream>
#include <sstream>
#include <map>
#include <cerrno>
#include <cstring>

#include "ServerConfig.hpp"
#include "JSON.hpp"
#include "Pair.hpp"
#include "VectorDebugger.hpp"

class ConfigParser
{
public:
    ConfigParser(const std::string &path);
    const std::vector<ServerConfig> &getConfigs() const;

private:
    std::string _json;
    std::vector<ServerConfig> _configs;
    void parse(const std::string &path);
    void output() const;

    // Exception class
    class ParsingException : public std::exception
    {
    public:
        ParsingException(const std::string &message);
        virtual ~ParsingException() throw() {};
        virtual const char *what() const throw();

    private:
        const std::string _message;
    };
};