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

        // Verify that on top of the JSON configuration object there is a key = servers
        if (!Pair::exist("servers", pairs))
            throw ParsingException("Expected 'servers' in the configuration file");

        // Get servers configuration
        std::vector<std::string> servers = JSON::getObjectsFromArray(Pair::get("servers", pairs).getValue());

        // Verify there is at least one object in the servers array
        if (servers.empty())
            throw ParsingException("Expected at least one server configuration in 'servers'");

        // Loop through every servers object
        for (std::vector<std::string>::const_iterator it = servers.begin(); it != servers.end(); it++)
        {
            pairs = JSON::getKeysAndValuesFromObject(*it);

            // Try to build ServerConfig object
            ServerConfig config(pairs);

            this->_configs.push_back(config);
        }
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