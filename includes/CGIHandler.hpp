#pragma once

#include <string>
#include <exception>
#include <map>
#include <sys/wait.h>

#include "Request.hpp"
#include "ServerConfig.hpp"

class CGIHandler
{
public:
    CGIHandler();
    void init(const ServerConfig &config, const Request &req, const Route *route, const std::string &path);
    void execute();
    const std::string &getByKey(const std::string &key) const;
    bool isActive(void) const;
    const pid_t &getPid(void) const;
    bool hasErrors(void) const;
    bool hasTimedOut(void) const;
    const std::string &getOutput(void) const;
    const std::string &getErrors(void) const;
    void readPipes(void);
    void clean(void);

private:
    std::map<std::string, std::string> _envs;
    std::string _output;
    std::string _errors;
    int _output_pipe;
    int _error_pipe;
    int _timeout_pipe;
    char **_envp;
    char **_args;
    pid_t _pid;
    bool _timedout;
    bool _active;
    void init_envp(void);
    void init_args(void);

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