#include "ConfigParser.hpp"

ConfigParser::ConfigParser(const std::string& path)
{
    this->parse(path);
}

const std::vector<ServerConfig>& ConfigParser::getConfigs() const
{
    return (this->_configs);
}

void ConfigParser::parse(const std::string& path)
{
    std::ifstream file;
    file.open(path.c_str());

    if (!file.is_open())
        throw ParsingException("Can't open configuration file");
    
    file.close();
}

ConfigParser::ParsingException::ParsingException(const std::string &message) : _message("Parsing Error - " + message)
{
    
}

const char *ConfigParser::ParsingException::what() const throw()
{
    return (_message.c_str());
}