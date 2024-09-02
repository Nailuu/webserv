#pragma once

#include <string>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <map>

#define DEFAULT_MAX_BODY_SIZE 10000
#define SERVER_NAME "Webserv/1.0.0"

#define WHITE "\033[0m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define RED "\033[31m"
#define BLUE "\033[94m"
#define GREY "\033[90m"

enum DATA_TYPES
{
    STRING,
    INT,
    ARRAY,
};

bool startsWith(const std::string &str, const std::string &prefix);
bool isValidDirectory(const std::string &path);
std::string highlight(const std::string& str);