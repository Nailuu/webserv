#pragma once

#include <string>

#define HTTP_METHODS (const char*[]) { "GET", "POST", "PUT", "DELETE" }
#define DEFAULT_MAX_BODY_SIZE 2000

enum HTTP_METHOD
{
    GET,
    POST,
    PUT,
    DELETE,
    UNKNOWN = -1
};

enum DATA_TYPES
{
    STRING,
    INT,
    ARRAY,
};

bool startsWith(const std::string& str, const std::string& prefix);

HTTP_METHOD getHttpMethodFromString(const std::string &str);
