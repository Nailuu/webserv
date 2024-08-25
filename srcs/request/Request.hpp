#pragma once

#include "../global.hpp"
#include <map>

class Request
{
    protected:
        const std::string _httpVersion;
        std::map<std::string, std::string> _fields;
    public:
        Request();
        virtual ~Request() = 0;
        Request(const std::string &httpVersion);
        Request(const Request &other);
        Request operator=(const Request &other);
        const std::string &getHttpVersion(void) const;
        const std::map<std::string, std::string> &getFields(void) const;
        void addField(const std::string &key, const std::string &value);
        class RequestException : public std::exception
        {
            public:
                RequestException(const std::string& message);
                virtual ~RequestException() throw() {};
                virtual const char* what() const throw();
            private:
                const std::string _message;
        };
};