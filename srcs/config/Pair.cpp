#include "Pair.hpp"

Pair::Pair(std::string &key, std::string &value) : _key(key), _value(value) {}

const std::string &Pair::getKey(void) const
{
    return (this->_key);
}

const std::string &Pair::getValue(void) const
{
    return (this->_value);
}

bool Pair::exist(const std::string &key, const std::vector<Pair> &pairs)
{
    for (std::vector<Pair>::const_iterator it = pairs.begin(); it != pairs.end(); it++)
    {
        if (it->getKey() == key)
            return (true);
    }

    return (false);
}

const Pair &Pair::get(const std::string &key, const std::vector<Pair> &pairs)
{
    for (std::vector<Pair>::const_iterator it = pairs.begin(); it != pairs.end(); it++)
    {
        if (it->getKey() == key)
            return (*it);
    }

    return (pairs.at(0));
}

std::ostream &operator<<(std::ostream &os, const Pair &p)
{
    os << "Key: " << p.getKey() << std::endl;
    os << "Value: " << std::endl;
    os << p.getValue() << std::endl;
    os << "----------------------------------";

    return (os);
}