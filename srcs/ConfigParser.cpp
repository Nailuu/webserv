#include "ConfigParser.hpp"

ConfigParser::ConfigParser(const std::string &path)
{
    this->parse(path);
}

const std::vector<ServerConfig> &ConfigParser::getConfigs() const
{
    return (this->_configs);
}

void ConfigParser::parse(const std::string &path)
{
    std::ifstream file;
    file.open(path.c_str());

    if (file.fail())
        throw ParsingException("Can't open configuration file: " + std::string(strerror(errno)));

    // Read file content
    std::stringstream buffer;
    buffer << file.rdbuf();

    file.close();

    // Sanitize the JSON by removing spaces and new lines
    this->_json = JSON::sanitize(buffer.str());

    try
    {
        std::vector<Pair> pairs = JSON::getKeysAndValuesFromObject(this->_json);

        // for (std::vector<Pair>::const_iterator it = pairs.begin(); it != pairs.end(); it++)
        //     std::cout << "Key: " << (*it).getKey() << "\n----------------------\nValue: " << (*it).getValue() << std::endl;

        if (!Pair::exist("servers", pairs))
            throw ParsingException("Expected 'servers' in the configuration file");
    }
    catch (std::exception &e)
    {
        throw;
    }
}

ConfigParser::ParsingException::ParsingException(const std::string &message) : _message("Parsing Error - " + message)
{
}

const char *ConfigParser::ParsingException::what() const throw()
{
    return (_message.c_str());
}