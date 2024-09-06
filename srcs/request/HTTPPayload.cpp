#include "HTTPPayload.hpp"

HTTPPayload::~HTTPPayload() {}

HTTPPayload::HTTPPayload(const std::string &httpVersion) : _httpVersion(httpVersion) {}

HTTPPayload::HTTPPayload(const HTTPPayload &other) : _httpVersion(other._httpVersion), _fields(other._fields) {}

const std::string &HTTPPayload::getHttpVersion(void) const
{
    return (this->_httpVersion);
}

const std::map<std::string, std::string> &HTTPPayload::getFields(void) const
{
    return (this->_fields);
}

const std::string &HTTPPayload::getFieldsValueByName(const std::string &key) const
{
    std::map<std::string, std::string>::const_iterator it = this->_fields.begin();
    for (; it != this->_fields.end(); it++)
    {
        if (it->first == key)
            return (it->second);
    }

    static const std::string empty = "";
    return empty;
}

void HTTPPayload::addField(const std::string &key, const std::string &value)
{
    this->_fields.insert(std::map<std::string, std::string>::value_type(key, value));
}

HTTPPayload::HTTPPayloadException::HTTPPayloadException(const std::string &message) : _message(std::string(RED) + "HTTPPayload Error" + std::string(YELLOW) + " - " + message) {}

const char *HTTPPayload::HTTPPayloadException::what() const throw()
{
    return (_message.c_str());
}
