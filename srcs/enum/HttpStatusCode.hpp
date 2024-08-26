#pragma once

#include "Enum.hpp"

class HttpStatusCode: public Enum<int>
{
    private:
        static std::vector<HttpStatusCode*> _types;
    public:
        HttpStatusCode();
        HttpStatusCode(const std::string &key, const int value);
        ~HttpStatusCode();
        HttpStatusCode(const HttpStatusCode &other);
        HttpStatusCode operator=(const HttpStatusCode &other);
        static bool addTypes(void);
        static const HttpStatusCode &get(const std::string &key);
        static const HttpStatusCode &getByValue(const int value);
        // 1xx - Informational
        static const HttpStatusCode CONTINUE;
        static const HttpStatusCode SWITCHING_PROTOCOLS;
        static const HttpStatusCode PROCESSING;

        // 2xx - Success
        static const HttpStatusCode OK;
        static const HttpStatusCode CREATED;
        static const HttpStatusCode ACCEPTED;
        static const HttpStatusCode NON_AUTHORITATIVE_INFORMATION;
        static const HttpStatusCode NO_CONTENT;
        static const HttpStatusCode RESET_CONTENT;
        static const HttpStatusCode PARTIAL_CONTENT;

        // 3xx - Redirection
        static const HttpStatusCode MULTIPLE_CHOICES;
        static const HttpStatusCode MOVED_PERMANENTLY;
        static const HttpStatusCode FOUND;
        static const HttpStatusCode SEE_OTHER;
        static const HttpStatusCode NOT_MODIFIED;
        static const HttpStatusCode TEMPORARY_REDIRECT;
        static const HttpStatusCode PERMANENT_REDIRECT;

        // 4xx - Client Error
        static const HttpStatusCode BAD_REQUEST;
        static const HttpStatusCode UNAUTHORIZED;
        static const HttpStatusCode PAYMENT_REQUIRED;
        static const HttpStatusCode FORBIDDEN;
        static const HttpStatusCode NOT_FOUND;
        static const HttpStatusCode METHOD_NOT_ALLOWED;
        static const HttpStatusCode NOT_ACCEPTABLE;
        static const HttpStatusCode PROXY_AUTHENTICATION_REQUIRED;
        static const HttpStatusCode REQUEST_TIMEOUT;
        static const HttpStatusCode CONFLICT;

        // 5xx - Server Error
        static const HttpStatusCode INTERNAL_SERVER_ERROR;
        static const HttpStatusCode NOT_IMPLEMENTED;
        static const HttpStatusCode BAD_GATEWAY;
        static const HttpStatusCode SERVICE_UNAVAILABLE;
        static const HttpStatusCode GATEWAY_TIMEOUT;
        static const HttpStatusCode HTTP_VERSION_NOT_SUPPORTED;

};
