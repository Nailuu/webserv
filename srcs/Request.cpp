#include "Request.hpp"

Request::~Request() {}

Request::Request(const HTTP_METHOD method, const std::string &path, const std::string &httpVersion, const std::string &host) : _method(method), _path(path), _httpVersion(httpVersion), _host(host) {}

Request::Request(const Request &other) : _method(other._method), _path(other._path), _httpVersion(other._httpVersion), _host(other._host), _fields(other._fields) {}

const std::string Request::extractAndValidate(std::string& str, const std::string& delimiter)
{
    unsigned long index = str.find(delimiter);

    if (index == std::string::npos || (index == 0 || str.length() <= delimiter.length())) {
        throw RequestException("Invalid line");
    }

    std::string result = str.substr(0, index);
    str = str.substr(index + delimiter.length());
    return result;
}

Request Request::fromString(std::string str)
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

    Request request((HTTP_METHOD)method, path, httpVersion, host);

    // Traitement des en-têtes
    while (str.length() > 0 && (str[0] != '\r' && str[0] != '\n')) {
        std::string key = extractAndValidate(str, ": ");
        std::string value = extractAndValidate(str, "\n");

        request.addField(key, value);
    }

    return request;
}

HTTP_METHOD Request::getMethod(void) const
{
    return (this->_method);
}

const std::string &Request::getPath(void) const
{
    return (this->_path);
}

const std::string &Request::getHttpVersion(void) const
{
    return (this->_httpVersion);
}

const std::string &Request::getHost(void) const
{
    return (this->_host);
}

const std::map<std::string, std::string> &Request::getFields(void) const
{
    return (this->_fields);
}

void Request::addField(const std::string &key, const std::string &value)
{
    this->_fields.insert(std::map<std::string, std::string>::value_type(key, value));
}

Request::RequestException::RequestException(const std::string &message) : _message("Request error - " + message)
{

}

const char* Request::RequestException::what() const throw()
{
    return (_message.c_str());
}
