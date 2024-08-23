#pragma once

#include "global.hpp"
#include <map>

class Request
{
    private:
        const HTTP_METHOD _method;
        const std::string _path;
        const std::string _httpVersion;
        const std::string _host;
        std::map<std::string, std::string> _fields;
        static const std::string extractAndValidate(std::string& str, const std::string& delimiter);
    public:
        Request();
        ~Request();
        Request(const HTTP_METHOD method, const std::string &path, const std::string &httpVersion, const std::string &host);
        Request(const Request &other);
        static Request fromString(std::string str);
        Request operator=(const Request &other);
        HTTP_METHOD getMethod(void) const;
        const std::string &getPath(void) const;
        const std::string &getHttpVersion(void) const;
        const std::string &getHost(void) const;
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
