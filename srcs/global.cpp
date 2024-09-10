#include "global.hpp"

#include <ctime>
#include <cstdlib>
#include <sstream>

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

std::string highlight(const std::string &str, bool yellow)
{
    return (std::string(BLUE) + str + (yellow ? std::string(YELLOW) : std::string(WHITE)));
}

std::string generateRandomId()
{
    srand(static_cast<unsigned int>(time(0)));

    std::ostringstream result;

    result << rand() % 9 + 1;

    for (int i = 1; i < 16; ++i)
        result << rand() % 10;

    return (result.str());
}