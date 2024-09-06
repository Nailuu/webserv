#pragma once

#include <string>
#include <vector>
#include <iostream>

class Pair
{
public:
    Pair(std::string &key, std::string &value);
    const std::string &getKey(void) const;
    const std::string &getValue(void) const;
    static bool exist(const std::string &key, const std::vector<Pair> &pairs);
    static const Pair &get(const std::string &key, const std::vector<Pair> &pairs);

private:
    std::string _key;
    std::string _value;
};

std::ostream &operator<<(std::ostream &os, const Pair &p);