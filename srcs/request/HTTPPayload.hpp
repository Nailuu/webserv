#pragma once

#include <map>
#include <string>
#include <exception>

#include "../global.hpp"

class HTTPPayload
{
public:
    virtual ~HTTPPayload() = 0;
    HTTPPayload(const std::string &httpVersion);
    HTTPPayload(const HTTPPayload &other);
    const std::string &getHttpVersion(void) const;
    const std::map<std::string, std::string> &getFields(void) const;
    void addField(const std::string &key, const std::string &value);

protected:
    const std::string _httpVersion;
    std::map<std::string, std::string> _fields;

    // Exception class
    class HTTPPayloadException : public std::exception
    {
    public:
        HTTPPayloadException(const std::string &message);
        virtual ~HTTPPayloadException() throw() {};
        virtual const char *what() const throw();

    private:
        const std::string _message;
    };
};