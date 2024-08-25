#include "Request.hpp"

Request::Request(const HTTP_METHOD method, const std::string &path, const std::string &httpVersion, const std::string &host) : HTTPPayload(httpVersion), _method(method), _path(path), _host(host) {}

Request::Request(const Request &other) : HTTPPayload(other), _method(other._method), _path(other._path), _host(other._host) {}

const std::string Request::extractAndValidate(std::string &str, const std::string &delimiter)
{
    unsigned long index = str.find(delimiter);

    if (index == std::string::npos || (index == 0 || str.length() <= delimiter.length()))
    {
        throw HTTPPayloadException("Invalid line: '" + str + "'");
    }

    std::string result = str.substr(0, index);
    str = str.substr(index + delimiter.length());
    return result;
}

Request Request::fromString(std::string str)
{
    std::string tmp = extractAndValidate(str, " ");
    HTTP_METHOD method = getHttpMethodFromString(tmp);

    if (method == UNKNOWN)
    {
        throw HTTPPayloadException("Invalid Method: '" + tmp + "'");
    }

    std::string path = extractAndValidate(str, " ");
    if (!startsWith(path, "/"))
    {
        throw HTTPPayloadException("Invalid Path: '" + path + "'");
    }

    std::string httpVersion = extractAndValidate(str, "\n");
    if (!startsWith(httpVersion, "HTTP/") || httpVersion.length() < 6)
    {
        throw HTTPPayloadException("Invalid HTTP Version: '" + httpVersion + "'");
    }

    std::string host = extractAndValidate(str, "\n");
    if (!startsWith(host, "Host: ") || host.length() <= 6)
    {
        throw HTTPPayloadException("Invalid Host: '" + host + "'");
    }
    host = host.substr(6); // remove "Host: "

    Request req((HTTP_METHOD)method, path, httpVersion, host);

    // Traitement des en-têtes
    while (str.length() > 0 && (str[0] != '\r' && str[0] != '\n'))
    {
        std::string key = extractAndValidate(str, ": ");
        std::string value = extractAndValidate(str, "\n");

        req.addField(key, value);
    }

    return req;
}

HTTP_METHOD Request::getMethod(void) const
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