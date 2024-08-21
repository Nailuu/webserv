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
    
    file.close();
    _servers.push_back(ServerConfig());
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

const std::vector<ServerConfig> GlobalConfig::getServers() const
{
    return (this->_servers);
}

GlobalConfig::InvalidConfigException::InvalidConfigException(const std::string& message)
{
    this->_message = "Invalid configuration - " + message;
}

const char* GlobalConfig::InvalidConfigException::what() const throw()
{
    return (this->_message.c_str());
}