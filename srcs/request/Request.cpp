#include "Request.hpp"

Request::~Request() {}

Request::Request(const std::string &httpVersion) : _httpVersion(httpVersion) {}

Request::Request(const Request &other) : _httpVersion(other._httpVersion), _fields(other._fields) {}

const std::string &Request::getHttpVersion(void) const
{
    return (this->_httpVersion);
}

const std::map<std::string, std::string> &Request::getFields(void) const
{
    return (this->_fields);
}

void Request::addField(const std::string &key, const std::string &value)
{
    this->_fields.insert(std::map<std::string, std::string>::value_type(key, value));
}

Request::RequestException::RequestException(const std::string &message) : _message("Request error - " + message) {}

const char* Request::RequestException::what() const throw()
{
    return (_message.c_str());
}
