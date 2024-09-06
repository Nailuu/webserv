#pragma once

#include <string>
#include <exception>
#include <map>

#include "Request.hpp"
#include "ServerConfig.hpp"

class CGIHandler
{
public:
    CGIHandler();
    void init(const ServerConfig &config, const Request &req, const Route *route, const std::string &path);
    const std::string execute(bool &timedout);
    const std::string &getByKey(const std::string &key) const;

private:
    std::map<std::string, std::string> _envs;
    char **_envp;
    char **_args;
    void init_envp(void);
    void init_args(void);
    void clean(void);

    // Exception class
    class CGIHandlerException : public std::exception
    {
    public:
        CGIHandlerException(const std::string &message);
        virtual ~CGIHandlerException() throw() {};
        virtual const char *what() const throw();

    private:
        const std::string _message;
    };
};