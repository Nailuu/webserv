#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cerrno>
#include <cstring>

#include "global.hpp"
#include "Route.hpp"
#include "Pair.hpp"
#include "JSON.hpp"
#include "AConfiguration.hpp"

class ServerConfig : public AConfiguration
{
public:
    ServerConfig();
    void build(const std::vector<Pair> &pairs);
    int getPort(void) const;
    const std::string &getHost(void) const;
    const std::string &getName(void) const;
    const std::vector<Route> &getRoutes(void) const;
    const Route *getRoute(const std::string &path, bool duplicate = false) const;

private:
    int _port;
    std::string _host;
    std::string _name;
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