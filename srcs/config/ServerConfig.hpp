#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>

#include "../global.hpp"
#include "Route.hpp"
#include "Pair.hpp"
#include "JSON.hpp"
#include "VectorDebugger.hpp"

class ServerConfig
{
public:
    ServerConfig(const std::vector<Pair> &pairs);
    ServerConfig(const ServerConfig &sc);
    ServerConfig &operator=(const ServerConfig &sc);
    int getPort() const;
    int getMaxBodySize() const;
    const std::string &getHost() const;
    const std::string &getName() const;
    const std::string &getRoot() const;
    const std::string &getIndex() const;
    const std::vector<HTTP_METHOD> &getHTTPMethods() const;
    const std::vector<Route> &getRoutes() const;

private:
    int _port;
    int _max_body_size;
    std::string _host;
    std::string _name;
    std::string _root;
    std::string _index;
    std::vector<HTTP_METHOD> _accepted_http_methods;
    std::vector<Route> _routes;
    void stringToInt(const std::string &str, int &result, const std::string &context);
    bool isValidDirectory(const std::string& path) const;
    void validate(const std::string &key, const std::vector<Pair> &pairs, std::string &result, bool mandatory = true);

    // Exception class
    class ServerConfigException : public std::exception
    {
    public:
        ServerConfigException(const std::string &message);
        virtual ~ServerConfigException() throw() {};
        virtual const char *what() const throw();

    private:
        const std::string _message;
    };
};