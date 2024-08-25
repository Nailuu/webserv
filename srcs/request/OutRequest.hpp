#pragma once

#include "Request.hpp"
#include "HttpStatusCode.hpp"

class OutRequest: public Request
{
    private:
        const HttpStatusCode _statusCode;
        const std::string _statusMessage;
        std::string _content;
        const std::string getCurrentTimeInHttpFormat() const;
    public:
        OutRequest();
        ~OutRequest();
        OutRequest(const std::string &httpVersion, const HttpStatusCode statusCode);
        OutRequest(const OutRequest &other);
        OutRequest operator=(const OutRequest &other);
        void setContent(const std::string &content);
        void setContentFile(const std::string &path);
        HttpStatusCode getStatusCode(void) const;
        const std::string &getStatusMessage(void) const;
        const std::string build(void) const;
};
