#pragma once

#include <string>
#include <exception>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <stdlib.h>

#include "../global.hpp"
#include "HTTPPayload.hpp"

class Response : public HTTPPayload
{
private:
    const HttpStatusCode _statusCode;
    const std::string _statusMessage;
    std::string _content;
    const std::string getCurrentTimeInHttpFormat() const;

public:
    Response(const std::string &httpVersion, const HttpStatusCode statusCode);
    Response(const Response &other);
    void setContent(const std::string &content);
    void setContentFile(const std::string &path);
    HttpStatusCode getStatusCode(void) const;
    const std::string &getStatusMessage(void) const;
    const std::string build(void) const;
};
