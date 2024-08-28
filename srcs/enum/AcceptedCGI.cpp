#include "AcceptedCGI.hpp"
#include <stdexcept>
#include <iostream>

std::vector<AcceptedCGI *> AcceptedCGI::_types;

const AcceptedCGI AcceptedCGI::PYTHON("python", ".py");

bool AcceptedCGI::addTypes(void)
{
    _types.push_back((AcceptedCGI *)&PYTHON);
    return (true);
}

const AcceptedCGI &AcceptedCGI::get(const std::string &key)
{
    std::vector<AcceptedCGI *>::iterator it = _types.begin();

    for (; it != _types.end(); it++)
    {
        AcceptedCGI *type = *it;

        if (type->_key == key)
        {
            return (*type);
        }
    }

    throw EnumException("Cannot get from Key");
}

const AcceptedCGI &AcceptedCGI::getByValue(const std::string &value)
{
    std::vector<AcceptedCGI *>::iterator it = _types.begin();

    for (; it != _types.end(); it++)
    {
        AcceptedCGI *type = *it;

        if (type->_value == value)
        {
            return (*type);
        }
    }

    throw EnumException("Cannot get from Value");
}

static bool added = AcceptedCGI::addTypes();

AcceptedCGI::AcceptedCGI(void) : Enum() {}

AcceptedCGI::AcceptedCGI(const std::string &key, const std::string &value) : Enum(key, value) {}

AcceptedCGI::~AcceptedCGI() {}

AcceptedCGI::AcceptedCGI(const AcceptedCGI &other) : Enum(other) {}

AcceptedCGI AcceptedCGI::operator=(const AcceptedCGI &other)
{
    if (this != &other)
    {
        _key = other._key;
        _value = other._value;
    }

    return (*this);
}

bool AcceptedCGI::operator==(const AcceptedCGI &other) const
{
    return (this->_key == other._key);
}