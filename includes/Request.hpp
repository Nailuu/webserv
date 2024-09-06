#pragma once

#include "HTTPPayload.hpp"
#include "HttpMethod.hpp"

class Request : public HTTPPayload
{
private:
    HttpMethod _method;
    std::string _route;
    std::string _path;
    std::string _params;
    std::string _host;
    static const std::string extractAndValidate(std::string &str, const std::string &delimiter);

public:
    Request();
    Request(const HttpMethod method, const std::string &path, const std::string &params, const std::string &httpVersion, const std::string &host);
    Request(const Request &other);
    Request operator=(const Request &other);
    static Request fromString(std::string &str);
    const HttpMethod &getMethod(void) const;
    const std::string &getPath(void) const;
    const std::string &getParams(void) const;
    const std::string &getHost(void) const;
};
