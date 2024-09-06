#pragma once

#include <string>
#include <vector>

#include "HttpMethod.hpp"

class AConfiguration
{
public:
    AConfiguration();
    virtual ~AConfiguration() = 0;
    int getMaxBodySize(void) const;
    const std::string &getRoot(void) const;
    const std::string &getIndex(void) const;
    const std::vector<HttpMethod> &getHTTPMethods(void) const;
    bool isHTTPMethodAuthorized(HttpMethod method) const;
    bool autoIndex(void) const;
    bool isAlias(void) const;

protected:
    int _max_body_size;
    bool _autoindex;
    bool _alias;
    std::string _root;
    std::string _index;
    std::vector<HttpMethod> _accepted_http_methods;
};