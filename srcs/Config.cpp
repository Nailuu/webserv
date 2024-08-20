#include "Config.hpp"

Config::Config(): _valid(false)
{

}

Config::Config(const std::string& config): _valid(false)
{
    // Check if the configuration file path is valid and the file can be open
    std::ifstream file;
    file.open(config.c_str());

    if (!file.is_open())
    {
        std::cerr << "Error: could not open configuration file" << std::endl;
        return ;
    }

    _valid = true;
}

Config::~Config()
{

}

bool Config::valid() const
{
    return (this->_valid);
}