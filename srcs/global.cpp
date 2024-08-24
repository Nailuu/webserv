#include "global.hpp"

bool startsWith(const std::string &str, const std::string &prefix)
{
    return (str.compare(0, prefix.size(), prefix) == 0);
}

HTTP_METHOD getHttpMethodFromString(const std::string &str)
{
    for (unsigned int i = 0; i < sizeof(HTTP_METHODS) / sizeof(char *); i++)
    {
        if (str == std::string(HTTP_METHODS[i]))
            return ((HTTP_METHOD)i);
    }

    return (UNKNOWN);
}

std::string getStringFromHttpMethod(HTTP_METHOD method)
{
    switch (method)
    {
    case GET:
        return ("GET");
    case POST:
        return ("POST");
    case PUT:
        return ("PUT");
    case DELETE:
        return ("DELETE");
    default:
        return ("");
    }
}

bool isValidDirectory(const std::string &path)
{
    struct stat info;

    if (stat(path.c_str(), &info) != 0)
        return (false);

    if (info.st_mode & S_IFDIR)
        return (true);

    return (false);
}