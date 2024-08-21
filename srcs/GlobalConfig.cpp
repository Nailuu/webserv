#include "GlobalConfig.hpp"

GlobalConfig::GlobalConfig()
{

}

GlobalConfig::GlobalConfig(const std::string& path)
{
    // Check if the GlobalConfiguration file path is valid and the file can be open
    std::ifstream file;
    file.open(path.c_str());

    if (!file.is_open())
        throw InvalidConfigException("Could not open configuration file");
}

GlobalConfig::GlobalConfig(const GlobalConfig& gc)
{
    *this = gc;
}

GlobalConfig::~GlobalConfig()
{

}

GlobalConfig& GlobalConfig::operator=(const GlobalConfig& gc)
{
    if (this != &gc)
    {
        this->_servers = gc._servers;
    }

    return (*this);
}

const ServerConfig& GlobalConfig::getServerConfig(int port) const
{
    for(std::vector<ServerConfig>::const_iterator it = this->_servers.begin(); it != this->_servers.end(); it++)
    {
        if ((*it).getPort() == port)
            return (*it);
    }

    return (*(this->_servers.end()));
}

GlobalConfig::InvalidConfigException::InvalidConfigException(const std::string& message)
{
    this->_message = "Invalid configuration - " + message;
}

const char* GlobalConfig::InvalidConfigException::what() const throw()
{
    return (this->_message.c_str());
}