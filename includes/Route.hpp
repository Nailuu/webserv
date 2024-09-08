#pragma once

#include <string>
#include <vector>
#include <exception>
#include <sstream>
#include <fstream>
#include <cerrno>
#include <cstring>

#include "global.hpp"
#include "Pair.hpp"
#include "JSON.hpp"
#include "HttpMethod.hpp"
#include "AConfiguration.hpp"

class Route : public AConfiguration
{
public:
    Route(int maxBodySize, const std::string &route, const std::string &root, const std::string &index, const std::vector<HttpMethod> &methods, bool autoindex, bool alias);
    void update(const std::vector<Pair> &pairs);
    const std::string &getRoute(void) const;
    const std::string &getRedirection(void) const;
    const std::vector<std::string> &getExtensions(void) const;
    const std::vector<std::string> &getExecutables(void) const;
    bool isCGI(void) const;

private:
    bool _cgi;
    int _selected_cgi;
    std::string _route;
    std::string _redirection;
    std::vector<std::string> _cgi_ext;
    std::vector<std::string> _cgi_exec;
    void stringToInt(const std::string &str, int &result, const std::string &context);
    void validate(const std::string &key, const std::vector<Pair> &pairs, std::string &result, bool mandatory = true);

    // Exception class
    class RouteException : public std::exception
    {
    public:
        RouteException(const std::string &message);
        virtual ~RouteException() throw() {};
        virtual const char *what() const throw();

    private:
        const std::string _message;
    };
};

std::ostream &operator<<(std::ostream &os, const Route &r);