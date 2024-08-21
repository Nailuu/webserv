#include "ServerConfig.hpp"

ServerConfig::ServerConfig(): _port(0), _host("default")
{

}

ServerConfig::ServerConfig(const ServerConfig& sc)
{
    *this = sc;
}

ServerConfig::~ServerConfig()
{

}

ServerConfig& ServerConfig::operator=(const ServerConfig& sc)
{
    if (this != &sc)
    {
        this->_port = sc._port;
        this->_host = sc._host;
        this->_accepted_http_methods = sc._accepted_http_methods;
    }

    return (*this);
}

int ServerConfig::getPort() const
{
    return (this->_port);
}