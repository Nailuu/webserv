#include "InRequest.hpp"

InRequest::~InRequest() {}

InRequest::InRequest(const HTTP_METHOD method, const std::string &path, const std::string &httpVersion, const std::string &host) : Request(httpVersion), _method(method), _path(path), _host(host) {}

InRequest::InRequest(const InRequest &other) : Request(other), _method(other._method), _path(other._path), _host(other._host) {}

const std::string InRequest::extractAndValidate(std::string& str, const std::string& delimiter)
{
    unsigned long index = str.find(delimiter);

    if (index == std::string::npos || (index == 0 || str.length() <= delimiter.length())) {
        throw RequestException("Invalid line");
    }

    std::string result = str.substr(0, index);
    str = str.substr(index + delimiter.length());
    return result;
}

InRequest InRequest::fromString(std::string str)
{
    HTTP_METHOD method = getHttpMethodFromString(extractAndValidate(str, " "));

    if (method == UNKNOWN) {
        throw RequestException("Invalid Method");
    }

    std::string path = extractAndValidate(str, " ");
    if (!startsWith(path, "/")) {
        throw RequestException("Invalid Path");
    }

    std::string httpVersion = extractAndValidate(str, "\n");
    if (!startsWith(httpVersion, "HTTP/") || httpVersion.length() < 6) {
        throw RequestException("Invalid HTTP Version");
    }

    std::string host = extractAndValidate(str, "\n");
    if (!startsWith(host, "Host: ") || host.length() <= 6) {
        throw RequestException("Invalid Host");
    }
    host = host.substr(6); // remove "Host: "

    InRequest InRequest((HTTP_METHOD)method, path, httpVersion, host);

    // Traitement des en-têtes
    while (str.length() > 0 && (str[0] != '\r' && str[0] != '\n')) {
        std::string key = extractAndValidate(str, ": ");
        std::string value = extractAndValidate(str, "\n");

        InRequest.addField(key, value);
    }

    return InRequest;
}

HTTP_METHOD InRequest::getMethod(void) const
{
    return (this->_method);
}

const std::string &InRequest::getPath(void) const
{
    return (this->_path);
}

const std::string &InRequest::getHost(void) const
{
    return (this->_host);
}
