#include "HttpMethod.hpp"
#include <stdexcept>
#include <iostream>

std::vector<HttpMethod*> HttpMethod::_types;

const HttpMethod HttpMethod::GET("GET", "GET");
const HttpMethod HttpMethod::POST("POST", "POST");
const HttpMethod HttpMethod::DELETE("DELETE", "DELETE");

bool HttpMethod::addTypes(void)
{
    _types.push_back((HttpMethod *) &GET);
    _types.push_back((HttpMethod *) &POST);
    _types.push_back((HttpMethod *) &DELETE);
    return (true);
}

const HttpMethod &HttpMethod::get(const std::string &key)
{
    std::vector<HttpMethod*>::iterator it = _types.begin();

    for (; it != _types.end(); it++)
    {
        HttpMethod *type = *it;

        if (type->_key == key) {
            return (*type);
        }
    }

    throw EnumException("Cannot get from Key");
}

const HttpMethod &HttpMethod::getByValue(const std::string &value)
{
    std::vector<HttpMethod*>::iterator it = _types.begin();

    for (; it != _types.end(); it++)
    {
        HttpMethod *type = *it;

        if (type->_value == value) {
            return (*type);
        }
    }

    throw EnumException("Cannot get from Value");
}

static bool added = HttpMethod::addTypes();

HttpMethod::HttpMethod(void) : Enum() {}

HttpMethod::HttpMethod(const std::string &key, const std::string &value) : Enum(key, value) {}

HttpMethod::~HttpMethod() {}

HttpMethod::HttpMethod(const HttpMethod &other) : Enum(other) {}

HttpMethod HttpMethod::operator=(const HttpMethod &other)
{
    if (this != &other)
    {
        _key = other._key;
        _value = other._value;
    }

    return (*this);
}
