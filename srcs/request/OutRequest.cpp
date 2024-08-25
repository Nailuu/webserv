#include "OutRequest.hpp"
#include <sstream>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <stdlib.h>

OutRequest::~OutRequest() {}

OutRequest::OutRequest(const std::string &httpVersion, const HttpStatusCode statusCode) : Request(httpVersion), _statusCode(statusCode), _statusMessage(GetStatusCodeMessage(statusCode)) {}

OutRequest::OutRequest(const OutRequest &other) : Request(other), _statusCode(other._statusCode), _statusMessage(other._statusMessage) {}

const std::string OutRequest::getCurrentTimeInHttpFormat() const
{
    std::time_t now = std::time(NULL);

    std::tm* gmtTime = std::gmtime(&now);

    char buffer[100];

    std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmtTime);

    return std::string(buffer);
}

void OutRequest::setContent(const std::string &content)
{
    _content = content;
}

void OutRequest::setContentFile(const std::string &path)
{
    std::ifstream file(path.c_str());

    if (!file.is_open()) {
        throw RequestException("Cannot open target file");
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    _content = buffer.str();

    file.close();
}

HttpStatusCode OutRequest::getStatusCode(void) const
{
    return (_statusCode);
}

const std::string &OutRequest::getStatusMessage(void) const
{
    return (_statusMessage);
}

const std::string OutRequest::build(void) const
{
    std::ostringstream oss;

    oss << _httpVersion << " " << _statusCode << " " << _statusMessage << "\r" << std::endl;

    // Adding mandatory headers
    oss << "Date: " << getCurrentTimeInHttpFormat() << "\r" << std::endl;
    oss << "Server: " << SERVER_NAME << "\r" << std::endl;
    oss << "Content-Length: " << _content.length() << "\r" << std::endl;

    //TODO: Need to be implement
    oss << "Content-Type: text/html; charset=UTF-8" << std::endl;

    std::map<std::string, std::string>::const_iterator it = _fields.begin();

    for (; it != _fields.end(); it++)
    {
        std::pair<std::string, std::string> info = *it;

        oss << info.first << ": " << info.second << "\r" << std::endl;
    }

    oss << std::endl << _content;
    return (oss.str());
}
