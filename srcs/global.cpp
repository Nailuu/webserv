#include "global.hpp"

bool startsWith(const std::string &str, const std::string &prefix)
{
    return (str.compare(0, prefix.size(), prefix) == 0);
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

std::string highlight(const std::string& str)
{
    return (std::string(BLUE) + str + std::string(YELLOW));
}