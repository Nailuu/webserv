#pragma once

#include <string>
#include <vector>

#include "../global.hpp"
#include "Pair.hpp"
#include "VectorDebugger.hpp"

class Route
{
public:
    Route(int maxBodySize, const std::string &route, const std::string &root, const std::string &index, const std::vector<HTTP_METHOD> &methods);
    void update(const std::vector<Pair>& pairs);
    int getMaxBodySize() const;
    const std::string &getRoute() const;
    const std::string &getRoot() const;
    const std::string &getIndex() const;
    const std::vector<HTTP_METHOD> &getHTTPMethods() const;

private:
    int _max_body_size;
    std::string _route;
    std::string _root;
    std::string _index;
    std::vector<HTTP_METHOD> _accepted_http_methods;

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