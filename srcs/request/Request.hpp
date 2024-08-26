#pragma once

#include "HTTPPayload.hpp"
#include "../enum/HttpMethod.hpp"

class Request : public HTTPPayload
{
private:
    const HttpMethod _method;
    const std::string _route;
    const std::string _host;
    static const std::string extractAndValidate(std::string &str, const std::string &delimiter);

public:
    Request(const HttpMethod method, const std::string &path, const std::string &httpVersion, const std::string &host);
    Request(const Request &other);
    static Request fromString(std::string &str);
    const HttpMethod &getMethod(void) const;
    const std::string &getPath(void) const;
    const std::string &getHost(void) const;
};
