#pragma once

#include "Enum.hpp"

class HttpMethod: public Enum<std::string>
{
    private:
        static std::vector<HttpMethod*> _types;
    public:
        HttpMethod();
        HttpMethod(const std::string &key, const std::string &value);
        ~HttpMethod();
        HttpMethod(const HttpMethod &other);
        HttpMethod operator=(const HttpMethod &other);
        static bool addTypes(void);
        static const HttpMethod &get(const std::string &key);
        static const HttpMethod &getByValue(const std::string &value);
        static const HttpMethod GET;
        static const HttpMethod POST;
        static const HttpMethod DELETE;
};
