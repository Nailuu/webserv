#include "JSON.hpp"

Pairs JSON::GetKeysAndValuesFromObject(const std::string& json)
{
    throw JSONException(json);
}

std::vector<std::string> JSON::GetObjectsFromArray(const std::string& json)
{
    throw JSONException(json);
}

JSON::JSONException::JSONException(const std::string &message) : _message("JSON Error - " + message)
{
    
}

const char* JSON::JSONException::what() const throw()
{
    return (_message.c_str());
}