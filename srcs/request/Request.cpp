#include "Request.hpp"
#include "../enum/HttpMethod.hpp"

Request::Request() : HTTPPayload(""), _method(HttpMethod::GET), _path(""), _host("") {} 

Request::Request(const HttpMethod method, const std::string &path, const std::string &httpVersion, const std::string &host) : HTTPPayload(httpVersion), _method(method), _path(path), _host(host) {}

Request::Request(const Request &other) : HTTPPayload(other), _method(other._method), _path(other._path), _host(other._host) {}

Request Request::operator=(const Request &other)
{
    if (this != &other)
    {
        _method = other._method;
        _route = other._route;
        _path = other._path;
        _host = other._host;
        _fields = other._fields;
    }
    return (*this);
}

const std::string Request::extractAndValidate(std::string &str, const std::string &delimiter)
{
    unsigned long index = str.find(delimiter);

    if (index == std::string::npos || (index == 0 || str.length() <= delimiter.length()))
    {
        throw HTTPPayloadException("Invalid line: '" + highlight(str) + "'");
    }

    std::string result = str.substr(0, index);
    str = str.substr(index + delimiter.length());
    return result;
}

Request Request::fromString(std::string &str)
{
    std::string tmp = extractAndValidate(str, " ");
    HttpMethod method;

    try
    {
        method = HttpMethod::get(tmp);
    }
    catch (const HttpMethod::EnumException &e)
    {
        throw HTTPPayloadException("Invalid Method: '" + highlight(tmp) + "'");
    }

    std::string route = extractAndValidate(str, " ");
    if (!startsWith(route, "/"))
    {
        throw HTTPPayloadException("Invalid Route: '" + highlight(route) + "'");
    }

    std::string httpVersion = extractAndValidate(str, "\n");
    if (!startsWith(httpVersion, "HTTP/") || httpVersion.length() < 6)
    {
        throw HTTPPayloadException("Invalid HTTP Version: '" + highlight(httpVersion) + "'");
    }

    std::string host = extractAndValidate(str, "\n");
    if (!startsWith(host, "Host: ") || host.length() <= 6)
    {
        throw HTTPPayloadException("Invalid Host: '" + highlight(host) + "'");
    }
    host = host.substr(6); // remove "Host: "

    Request req((HttpMethod)method, route, httpVersion, host);

    // Traitement des en-têtes
    while (str.length() > 0 && (str[0] != '\r' && str[0] != '\n'))
    {
        std::string key = extractAndValidate(str, ": ");
        std::string value = extractAndValidate(str, "\n");

        req.addField(key, value);
    }

    return req;
}

const HttpMethod &Request::getMethod(void) const
{
    return (this->_method);
}

const std::string &Request::getPath(void) const
{
    return (this->_path);
}

const std::string &Request::getHost(void) const
{
    return (this->_host);
}
