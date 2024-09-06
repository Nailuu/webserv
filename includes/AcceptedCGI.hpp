#pragma once

#include "Enum.hpp"

class AcceptedCGI : public Enum<std::string>
{
private:
    static std::vector<AcceptedCGI *> _types;

public:
    AcceptedCGI();
    AcceptedCGI(const std::string &key, const std::string &value);
    ~AcceptedCGI();
    AcceptedCGI(const AcceptedCGI &other);
    AcceptedCGI operator=(const AcceptedCGI &other);
    bool operator==(const AcceptedCGI &other) const;
    static bool addTypes(void);
    static const AcceptedCGI &get(const std::string &key);
    static const AcceptedCGI &getByValue(const std::string &value);
    static const AcceptedCGI PYTHON;
    static const AcceptedCGI BLA;
};
