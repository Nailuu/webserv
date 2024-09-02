#include "AutoIndexGenerator.hpp"

#include <sstream>
#include <dirent.h>
#include <sys/types.h>
#include <iostream>
#include <cerrno>
#include <cstring>
#include <sys/stat.h>

#include "../global.hpp"

AutoIndexGenerator::~AutoIndexGenerator() {};

const std::string AutoIndexGenerator::generate(const std::string &directory, const std::string &route)
{
    std::ostringstream html;

    DIR *dir = opendir(directory.c_str());
    if (dir == NULL)
        throw AutoIndexGeneratorException("Can't open directory '" + highlight(directory) + "': '" + highlight(std::string(strerror(errno))) + "'");

    // Add CSS styles, and html headers
    html << "<!DOCTYPE html> <html lang=\"en\"> <head> <meta charset=\"UTF-8\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> <title>Directory Listing</title> <style> body { font-family: Arial, sans-serif; padding: 20px; background-color: #f7f7f7; } h1 { font-size: 24px; margin-bottom: 20px; } table { width: 100%; border-collapse: collapse; margin-bottom: 20px; } th, td { padding: 8px 12px; border: 1px solid #ddd; text-align: left; } th { background-color: #f2f2f2; } a { color: #007bff; text-decoration: none; } a:hover { text-decoration: underline; } .size { text-align: right; } </style> </head> <body>";

    // Title
    html << "<h1>Index of " << directory << "</h1>";

    // Table header
    html << "<table><thead><tr><th>Name</th><th>Last modified</th><th class=\"size\">Size</th></tr></thead><tbody>";

    // Add every directories/files
    struct dirent *entry;
    struct stat fileStat;

    while ((entry = readdir(dir)) != NULL)
    {
        // Skip the "." and ".." entries
        if (entry->d_name[0] == '.')
            continue;

        std::string path = std::string(directory) + "/" + entry->d_name;

        // Use stat() to get the file status
        if (stat(path.c_str(), &fileStat) == -1)
            throw AutoIndexGeneratorException("Could not get file status with stat(): '" + highlight(std::string(entry->d_name)) + "'");

        char timeBuffer[80];
        struct tm *timeinfo;

        // Get last modified time
        timeinfo = localtime(&fileStat.st_mtime);

        // Format the time
        strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", timeinfo);

        // Directories
        if (S_ISDIR(fileStat.st_mode))
            html << "<tr><td><a href=\"" << route << entry->d_name << "/\">" << entry->d_name << "/</a></td><td>" << timeBuffer << "</td><td class=\"size\">-</td></tr>";

        // Files
        else if (S_ISREG(fileStat.st_mode))
            html << "<tr><td><a href=\"" << route << entry->d_name << "\">" << entry->d_name << "</a></td><td>" << timeBuffer << "</td><td class=\"size\">" << fileStat.st_size << " bytes" << "</td></tr>";
    }

    // Close last tags
    html << "</tbody></table><address>";
    html << SERVER_NAME << "</address>";
    html << "</body></html>";

    closedir(dir);
    return (html.str());
}

AutoIndexGenerator::AutoIndexGeneratorException::AutoIndexGeneratorException(const std::string &message) : _message(std::string(RED) + "AutoIndexGenerator Error" + std::string(YELLOW) + " - " + message)
{
}

const char *AutoIndexGenerator::AutoIndexGeneratorException::what() const throw()
{
    return (_message.c_str());
}