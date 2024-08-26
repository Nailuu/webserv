#pragma once

#include <string>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define HTTP_METHODS \
    (const char *[]) { "GET", "POST", "DELETE" }
#define DEFAULT_MAX_BODY_SIZE 2000
#define SERVER_NAME "Webserv/1.0.0"

enum HTTP_METHOD
{
    GET,
    POST,
    DELETE,
    UNKNOWN = -1
};

enum DATA_TYPES
{
    STRING,
    INT,
    ARRAY,
};

enum HttpStatusCode
{
    // 1xx - Informational
    Continue = 100,
    SwitchingProtocols = 101,
    Processing = 102,

    // 2xx - Success
    OK = 200,
    Created = 201,
    Accepted = 202,
    NonAuthoritativeInformation = 203,
    NoContent = 204,
    ResetContent = 205,
    PartialContent = 206,

    // 3xx - Redirection
    MultipleChoices = 300,
    MovedPermanently = 301,
    Found = 302,
    SeeOther = 303,
    NotModified = 304,
    TemporaryRedirect = 307,
    PermanentRedirect = 308,

    // 4xx - Client Error
    BadRequest = 400,
    Unauthorized = 401,
    PaymentRequired = 402,
    Forbidden = 403,
    NotFound = 404,
    MethodNotAllowed = 405,
    NotAcceptable = 406,
    ProxyAuthenticationRequired = 407,
    RequestTimeout = 408,
    Conflict = 409,

    // 5xx - Server Error
    InternalServerError = 500,
    NotImplemented = 501,
    BadGateway = 502,
    ServiceUnavailable = 503,
    GatewayTimeout = 504,
    HTTPVersionNotSupported = 505
};

bool startsWith(const std::string &str, const std::string &prefix);
HTTP_METHOD getHttpMethodFromString(const std::string &str);
std::string getStringFromHttpMethod(HTTP_METHOD method);
bool isValidDirectory(const std::string &path);
std::string GetStatusCodeMessage(HttpStatusCode code);
