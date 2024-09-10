#pragma once

#include <string>
#include <map>

class CookiesManager
{
public:
    CookiesManager();
    const std::string generateCookiePage(const std::string &id) const;
    const std::string &newCookie(const std::string &data);

private:
    std::map<std::string, std::string> _cookies;
};