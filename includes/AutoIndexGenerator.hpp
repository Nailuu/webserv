#pragma once

#include <string>

class AutoIndexGenerator
{
public:
    virtual ~AutoIndexGenerator() = 0;
    static const std::string generate(const std::string &directory, const std::string &route);

    // Exception class
    class AutoIndexGeneratorException : public std::exception
    {
    public:
        AutoIndexGeneratorException(const std::string &message);
        virtual ~AutoIndexGeneratorException() throw() {};
        virtual const char *what() const throw();

    private:
        const std::string _message;
    };
};