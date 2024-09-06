#pragma once

#include <string>
#include <exception>
#include <map>

#include "Request.hpp"
#include "ServerConfig.hpp"
#include "CGI.hpp"

class CGIENV
{
public:
    CGIENV();
    CGIENV(const ServerConfig &config, const Request &req, const CGI *cgi);
    ~CGIENV();
    CGIENV operator=(const CGIENV &other);
    void print(void) const;
    char **getEnvs(void);
    const std::string &getByKey(const std::string &key) const;

private:
    std::map<std::string, std::string> _envs;
    char **_c_envs;

    // Exception class
    class CGIENVException : public std::exception
    {
    public:
        CGIENVException(const std::string &message);
        virtual ~CGIENVException() throw() {};
        virtual const char *what() const throw();

    private:
        const std::string _message;
    };
};