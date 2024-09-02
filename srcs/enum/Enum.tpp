#include "Enum.hpp"
#include <stdexcept>
#include <algorithm>
#include <ostream>
#include <iostream>

template <typename T>
Enum<T>::Enum() {}

template <typename T>
Enum<T>::Enum(const std::string &key, const T &value) : _key(key), _value(value) {}

template <typename T>
Enum<T>::~Enum() {}


template <typename T>
Enum<T>::Enum(const Enum &other) : _key(other._key), _value(other._value) {}

template <typename T>
std::ostream &operator<<(std::ostream &os, const Enum<T> &e)
{
    os << e.getKey();

    return (os);
}

template <typename T>
const std::string &Enum<T>::getKey(void) const
{
    return (this->_key);
}

template <typename T>
const T &Enum<T>::getValue(void) const
{
    return (this->_value);
}

template <typename T>
Enum<T>::EnumException::EnumException(const std::string &message) : _message(std::string(RED) + "Enum Error" + std::string(YELLOW) + " - " + message) {}

template <typename T>
const char *Enum<T>::EnumException::what() const throw()
{
    return (_message.c_str());
}
