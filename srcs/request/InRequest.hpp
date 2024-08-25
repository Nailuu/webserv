#pragma once

#include "Request.hpp"

class InRequest: public Request
{
    private:
        const HTTP_METHOD _method;
        const std::string _path;
        const std::string _host;
        static const std::string extractAndValidate(std::string& str, const std::string& delimiter);
    public:
        InRequest();
        ~InRequest();
        InRequest(const HTTP_METHOD method, const std::string &path, const std::string &httpVersion, const std::string &host);
        InRequest(const InRequest &other);
        static InRequest fromString(std::string str);
        InRequest operator=(const InRequest &other);
        HTTP_METHOD getMethod(void) const;
        const std::string &getPath(void) const;
        const std::string &getHost(void) const;
};
