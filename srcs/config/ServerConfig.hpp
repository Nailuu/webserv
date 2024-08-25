#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
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
    ServerConfig();
    void build(const std::vector<Pair> &pairs);
    int getPort(void) const;
    int getMaxBodySize(void) const;
    const std::string &getHost(void) const;
    const std::string &getName(void) const;
    const std::string &getRoot(void) const;
    const std::string &getIndex(void) const;
    const std::vector<HTTP_METHOD> &getHTTPMethods(void) const;
    const std::vector<Route> &getRoutes(void) const;

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

std::ostream &operator<<(std::ostream &os, const ServerConfig &sc);