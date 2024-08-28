#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <cerrno>
#include <cstring>

#include "../enum/HttpMethod.hpp"
#include "../enum/AcceptedCGI.hpp"
#include "../global.hpp"
#include "Pair.hpp"
#include "JSON.hpp"

class CGI
{
public:
    CGI(const std::string &root, const std::vector<HttpMethod> &methods);
    void update(const std::vector<Pair> &pairs);

private:
    std::string _type;
    std::string _root;
    std::string _exec;
    std::vector<HttpMethod> _accepted_http_methods;
    void validate(const std::string &key, const std::vector<Pair> &pairs, std::string &result, bool mandatory = true);

    // Exception class
    class CGIException : public std::exception
    {
    public:
        CGIException(const std::string &message);
        virtual ~CGIException() throw() {};
        virtual const char *what() const throw();

    private:
        const std::string _message;
    };
};