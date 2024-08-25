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
#include "../enum/MimeType.hpp"
#include "../enum/HttpStatusCode.hpp"

class Response : public HTTPPayload
{
private:
    const HttpStatusCode &_statusCode;
    std::string _content;
    MimeType _mimeType;
    const std::string getCurrentTimeInHttpFormat() const;

public:
    Response(const std::string &httpVersion, const HttpStatusCode &statusCode);
    Response(const Response &other);
    Response operator=(const Response &other);
    void setContent(const std::string &content, const MimeType &mimeType);
    void setContentFile(const std::string &path);
    const HttpStatusCode &getStatusCode(void) const;
    const std::string build(void) const;
};
