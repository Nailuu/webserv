#pragma once

#include <string>
#include <vector>
#include <exception>
#include <sstream>
#include <fstream>
#include <cerrno>
#include <cstring>

#include "../global.hpp"
#include "Pair.hpp"
#include "VectorDebugger.hpp"
#include "JSON.hpp"
#include "../enum/HttpMethod.hpp"

class Route
{
public:
    Route(int maxBodySize, const std::string &route, const std::string &root, const std::string &index, const std::vector<HttpMethod> &methods);
    void update(const std::vector<Pair>& pairs);
    int getMaxBodySize(void) const;
    const std::string &getRoute(void) const;
    const std::string &getRoot(void) const;
    const std::string &getIndex(void) const;
    const std::vector<HttpMethod> &getHTTPMethods(void) const;
    bool isHTTPMethodAuthorized(HttpMethod method) const;

private:
    int _max_body_size;
    std::string _route;
    std::string _root;
    std::string _index;
    std::vector<HttpMethod> _accepted_http_methods;
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

std::ostream& operator<<(std::ostream& os, const Route& r);