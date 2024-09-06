#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <cerrno>
#include <cstring>

#include "HttpMethod.hpp"
#include "AcceptedCGI.hpp"
#include "global.hpp"
#include "Pair.hpp"
#include "JSON.hpp"
#include "AConfiguration.hpp"

class CGI : public AConfiguration
{
public:
    CGI(const std::string &root, const std::vector<HttpMethod> &methods);
    void update(const std::vector<Pair> &pairs);
    const std::string &getType(void) const;
    const std::string &getExec(void) const;

private:
    std::string _type;
    std::string _exec;
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

std::ostream& operator<<(std::ostream& os, const CGI& c);