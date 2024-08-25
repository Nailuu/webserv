#pragma once

#include <string>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define HTTP_METHODS (const char*[]) { "GET", "POST", "PUT", "DELETE" }
#define DEFAULT_MAX_BODY_SIZE 2000
#define SERVER_NAME "Webserv/1.0.0"

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
std::string getStringFromHttpMethod(HTTP_METHOD method);
bool isValidDirectory(const std::string& path);
