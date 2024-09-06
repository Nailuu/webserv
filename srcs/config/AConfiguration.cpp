#include "AConfiguration.hpp"

AConfiguration::AConfiguration() : _max_body_size(0), _autoindex(false), _alias(false) {}

AConfiguration::~AConfiguration() {}

int AConfiguration::getMaxBodySize(void) const
{
    return (this->_max_body_size);
}

const std::string &AConfiguration::getRoot(void) const
{
    return (this->_root);
}

const std::string &AConfiguration::getIndex(void) const
{
    return (this->_index);
}

const std::vector<HttpMethod> &AConfiguration::getHTTPMethods(void) const
{
    return (this->_accepted_http_methods);
}

bool AConfiguration::isHTTPMethodAuthorized(HttpMethod method) const
{
    std::vector<HttpMethod>::const_iterator it = this->_accepted_http_methods.begin();
    for (; it != this->_accepted_http_methods.end(); it++)
    {
        if ((*it) == method)
            return (true);
    }

    return (false);
}

bool AConfiguration::autoIndex(void) const
{
    return (this->_autoindex);
}

bool AConfiguration::isAlias(void) const
{
    return (this->_alias);
}