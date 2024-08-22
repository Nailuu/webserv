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

    // Read file content
    std::stringstream buffer;
    buffer << file.rdbuf();

    file.close();

    this->_json = buffer.str();

    // Sanitize the JSON by removing spaces and new lines
    this->sanitize();


    /*try {
        Pairs p = JSON::GetKeysAndValuesFromObject(this->_json);
    } catch(std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return ;
    }*/
   _configs.push_back(ServerConfig());
}

void ConfigParser::sanitize()
{
    std::size_t pos;
    while ((pos = this->_json.find_first_of(" \n")) != std::string::npos)
        this->_json.erase(pos, 1);
}

ConfigParser::ParsingException::ParsingException(const std::string &message) : _message("Parsing Error - " + message)
{
    
}

const char* ConfigParser::ParsingException::what() const throw()
{
    return (_message.c_str());
}