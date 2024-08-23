#include "global.hpp"

inline bool startsWith(const std::string& str, const std::string& prefix)
{
    return (str.compare(0, prefix.size(), prefix) == 0);
}

HTTP_METHOD getHttpMethodFromString(const std::string &str)
{
    for (unsigned int i = 0; i < sizeof(HTTP_METHODS) / sizeof(char *); i++)
    {
        if (startsWith(str, HTTP_METHODS[i]))
            return ((HTTP_METHOD) i);
    }

    return (UNKNOWN);
}
