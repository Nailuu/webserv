#pragma once

#include "HTTPPayload.hpp"

class Request : public HTTPPayload
{
private:
    const HTTP_METHOD _method;
    const std::string _path;
    const std::string _host;
    static const std::string extractAndValidate(std::string &str, const std::string &delimiter);

public:
    Request(const HTTP_METHOD method, const std::string &path, const std::string &httpVersion, const std::string &host);
    Request(const Request &other);
    static Request fromString(std::string str);
    HTTP_METHOD getMethod(void) const;
    const std::string &getPath(void) const;
    const std::string &getHost(void) const;
};
