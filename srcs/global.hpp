#pragma once

#include <string>

#define HTTP_METHODS (const char*[]) { "GET", "POST", "PUT", "DELETE" }

enum HTTP_METHOD
{
    GET,
    POST,
    PUT,
    DELETE,
    UNKNOWN = -1
};

bool startsWith(const std::string& str, const std::string& prefix);

HTTP_METHOD getHttpMethodFromString(const std::string &str);
