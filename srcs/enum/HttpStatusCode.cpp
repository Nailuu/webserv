#include "HttpStatusCode.hpp"
#include <stdexcept>
#include <iostream>

std::vector<HttpStatusCode*> HttpStatusCode::_types;

// 1xx - Informational
const HttpStatusCode HttpStatusCode::CONTINUE("CONTINUE", 100);
const HttpStatusCode HttpStatusCode::SWITCHING_PROTOCOLS("SWITCHING_PROTOCOLS", 101);
const HttpStatusCode HttpStatusCode::PROCESSING("PROCESSING", 102);

// 2xx - Success
const HttpStatusCode HttpStatusCode::OK("OK", 200);
const HttpStatusCode HttpStatusCode::CREATED("CREATED", 201);
const HttpStatusCode HttpStatusCode::ACCEPTED("ACCEPTED", 202);
const HttpStatusCode HttpStatusCode::NON_AUTHORITATIVE_INFORMATION("NON_AUTHORITATIVE_INFORMATION", 203);
const HttpStatusCode HttpStatusCode::NO_CONTENT("NO_CONTENT", 204);
const HttpStatusCode HttpStatusCode::RESET_CONTENT("RESET_CONTENT", 205);
const HttpStatusCode HttpStatusCode::PARTIAL_CONTENT("PARTIAL_CONTENT", 206);

// 3xx - Redirection
const HttpStatusCode HttpStatusCode::MULTIPLE_CHOICES("MULTIPLE_CHOICES", 300);
const HttpStatusCode HttpStatusCode::MOVED_PERMANENTLY("MOVED_PERMANENTLY", 301);
const HttpStatusCode HttpStatusCode::FOUND("FOUND", 302);
const HttpStatusCode HttpStatusCode::SEE_OTHER("SEE_OTHER", 303);
const HttpStatusCode HttpStatusCode::NOT_MODIFIED("NOT_MODIFIED", 304);
const HttpStatusCode HttpStatusCode::TEMPORARY_REDIRECT("TEMPORARY_REDIRECT", 307);
const HttpStatusCode HttpStatusCode::PERMANENT_REDIRECT("PERMANENT_REDIRECT", 308);

// 4xx - Client Error
const HttpStatusCode HttpStatusCode::BAD_REQUEST("BAD_REQUEST", 400);
const HttpStatusCode HttpStatusCode::UNAUTHORIZED("UNAUTHORIZED", 401);
const HttpStatusCode HttpStatusCode::PAYMENT_REQUIRED("PAYMENT_REQUIRED", 402);
const HttpStatusCode HttpStatusCode::FORBIDDEN("FORBIDDEN", 403);
const HttpStatusCode HttpStatusCode::NOT_FOUND("NOT_FOUND", 404);
const HttpStatusCode HttpStatusCode::METHOD_NOT_ALLOWED("METHOD_NOT_ALLOWED", 405);
const HttpStatusCode HttpStatusCode::NOT_ACCEPTABLE("NOT_ACCEPTABLE", 406);
const HttpStatusCode HttpStatusCode::PROXY_AUTHENTICATION_REQUIRED("PROXY_AUTHENTICATION_REQUIRED", 407);
const HttpStatusCode HttpStatusCode::REQUEST_TIMEOUT("REQUEST_TIMEOUT", 408);
const HttpStatusCode HttpStatusCode::CONFLICT("CONFLICT", 409);

// 5xx - Server Error
const HttpStatusCode HttpStatusCode::INTERNAL_SERVER_ERROR("INTERNAL_SERVER_ERROR", 500);
const HttpStatusCode HttpStatusCode::NOT_IMPLEMENTED("NOT_IMPLEMENTED", 501);
const HttpStatusCode HttpStatusCode::BAD_GATEWAY("BAD_GATEWAY", 502);
const HttpStatusCode HttpStatusCode::SERVICE_UNAVAILABLE("SERVICE_UNAVAILABLE", 503);
const HttpStatusCode HttpStatusCode::GATEWAY_TIMEOUT("GATEWAY_TIMEOUT", 504);
const HttpStatusCode HttpStatusCode::HTTP_VERSION_NOT_SUPPORTED("HTTP_VERSION_NOT_SUPPORTED", 505);

// HttpStatusCode.cpp

bool HttpStatusCode::addTypes() {
    _types.push_back((HttpStatusCode *)&CONTINUE);
    _types.push_back((HttpStatusCode *)&SWITCHING_PROTOCOLS);
    _types.push_back((HttpStatusCode *)&PROCESSING);

    _types.push_back((HttpStatusCode *)&OK);
    _types.push_back((HttpStatusCode *)&CREATED);
    _types.push_back((HttpStatusCode *)&ACCEPTED);
    _types.push_back((HttpStatusCode *)&NON_AUTHORITATIVE_INFORMATION);
    _types.push_back((HttpStatusCode *)&NO_CONTENT);
    _types.push_back((HttpStatusCode *)&RESET_CONTENT);
    _types.push_back((HttpStatusCode *)&PARTIAL_CONTENT);

    _types.push_back((HttpStatusCode *)&MULTIPLE_CHOICES);
    _types.push_back((HttpStatusCode *)&MOVED_PERMANENTLY);
    _types.push_back((HttpStatusCode *)&FOUND);
    _types.push_back((HttpStatusCode *)&SEE_OTHER);
    _types.push_back((HttpStatusCode *)&NOT_MODIFIED);
    _types.push_back((HttpStatusCode *)&TEMPORARY_REDIRECT);
    _types.push_back((HttpStatusCode *)&PERMANENT_REDIRECT);

    _types.push_back((HttpStatusCode *)&BAD_REQUEST);
    _types.push_back((HttpStatusCode *)&UNAUTHORIZED);
    _types.push_back((HttpStatusCode *)&PAYMENT_REQUIRED);
    _types.push_back((HttpStatusCode *)&FORBIDDEN);
    _types.push_back((HttpStatusCode *)&NOT_FOUND);
    _types.push_back((HttpStatusCode *)&METHOD_NOT_ALLOWED);
    _types.push_back((HttpStatusCode *)&NOT_ACCEPTABLE);
    _types.push_back((HttpStatusCode *)&PROXY_AUTHENTICATION_REQUIRED);
    _types.push_back((HttpStatusCode *)&REQUEST_TIMEOUT);
    _types.push_back((HttpStatusCode *)&CONFLICT);

    _types.push_back((HttpStatusCode *)&INTERNAL_SERVER_ERROR);
    _types.push_back((HttpStatusCode *)&NOT_IMPLEMENTED);
    _types.push_back((HttpStatusCode *)&BAD_GATEWAY);
    _types.push_back((HttpStatusCode *)&SERVICE_UNAVAILABLE);
    _types.push_back((HttpStatusCode *)&GATEWAY_TIMEOUT);
    _types.push_back((HttpStatusCode *)&HTTP_VERSION_NOT_SUPPORTED);

    return (true);
}


const HttpStatusCode &HttpStatusCode::get(const std::string &key)
{
    std::vector<HttpStatusCode*>::iterator it = _types.begin();

    for (; it != _types.end(); it++)
    {
        HttpStatusCode *type = *it;

        if (type->_key == key) {
            return (*type);
        }
    }

    throw EnumException("Cannot get from Key");
}

const HttpStatusCode &HttpStatusCode::getByValue(const int value)
{
    std::vector<HttpStatusCode*>::iterator it = _types.begin();

    for (; it != _types.end(); it++)
    {
        HttpStatusCode *type = *it;

        if (type->_value == value) {
            return (*type);
        }
    }

    throw EnumException("Cannot get from Value");
}

static bool added = HttpStatusCode::addTypes();

HttpStatusCode::HttpStatusCode(void) : Enum() {}

HttpStatusCode::HttpStatusCode(const std::string &key, const int value) : Enum(key, value) {}

HttpStatusCode::~HttpStatusCode() {}

HttpStatusCode::HttpStatusCode(const HttpStatusCode &other) : Enum(other) {}

HttpStatusCode HttpStatusCode::operator=(const HttpStatusCode &other)
{
    if (this != &other)
    {
        _key = other._key;
        _value = other._value;
    }

    return (*this);
}
