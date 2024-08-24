#include "JSON.hpp"

std::size_t JSON::getClosingBracketPos(const std::string &json, std::string bracket_type)
{
    // open curly bracket and close curly bracket count
    int ob = 0, cb = 0;

    std::size_t pos = -1;

    // Loop until position of the corresponding closing bracket is found
    while ((pos = json.find_first_of(bracket_type, pos + 1)) != std::string::npos)
    {
        if (json.at(pos) == bracket_type.at(0))
            ob++;
        else
            cb++;

        if (cb == ob)
            break;
    }

    return (pos);
}

// Extract array content without square bracket at the beginning and end
std::string JSON::getContentFromArray(const std::string &json)
{
    if (json.at(0) != '[')
        throw JSONException("Expected '['\n" + beautify(json));

    // Get position of closing square bracket
    std::size_t pos = getClosingBracketPos(json, "[]");

    if (pos == std::string::npos)
        throw JSONException("Array is never closed ']'\n" + beautify(json));

    std::string array = json.substr(1, pos - 1);

    return (array);
}

std::vector<std::string> JSON::getObjectsFromArray(const std::string &json)
{
    try
    {
        std::vector<std::string> result;

        std::string array = getContentFromArray(json);

        if (array.empty())
            return (result);

        while (true)
        {
            // if first character is a comma, skip it
            if (array.at(0) == ',')
                array.erase(0, 1);

            // if after comma string is empty break because parsing is done
            if (array.empty())
                break;

            if (array.at(0) != '{')
                throw JSONException("Expected '{'\n" + beautify(array));

            // Get position of closing curly bracket
            std::size_t pos = getClosingBracketPos(array, "{}");

            if (pos == std::string::npos)
                throw JSONException("Object is never closed '}'\n" + beautify(array));

            std::string object = array.substr(0, pos + 1);
            if (object.length() == 2)
                throw JSONException("Object is empty!\n" + beautify(array));

            // Push full object including open and close curly braces to vector
            result.push_back(object);

            // Check that after a closing curly bracket there is a comma
            if ((array.length() - 1) == pos || array.at(pos + 1) != ',')
                throw JSONException("Expected ',' after object\n" + beautify(array));

            array = array.substr(pos + 1);
        }

        return (result);
    }
    catch (std::exception &e)
    {
        throw;
    }
}

std::vector<Pair> JSON::getKeysAndValuesFromObject(const std::string &json)
{
    if (json.at(0) != '{')
        throw JSONException("Expected '{'\n" + beautify(json));

    if (json.at(json.length() - 1) != '}')
        throw JSONException("Object is never closed '}'\n" + beautify(json));

    if (json.length() == 2)
        throw JSONException("Object is empty!\n" + beautify(json));

    std::vector<Pair> result;

    std::string tmp = json;

    // Remove curly braces {}
    tmp.erase(0, 1);
    tmp.erase(tmp.length() - 1);

    while (true)
    {
        // if first character is a comma or opening curly bracket, skip it
        if (tmp.at(0) == ',')
            tmp.erase(0, 1);

        // if after comma string is empty break because parsing is done
        if (tmp.empty())
            break;

        std::string key, value;

        std::size_t pos;
        pos = tmp.find(':');

        if (pos == std::string::npos)
            throw JSONException("Expected ':'\n" + beautify(tmp));

        key = tmp.substr(0, pos);
        tmp = tmp.substr(pos + 1);

        if (tmp.at(0) == '[')
        {
            // Get position of closing square bracket
            pos = getClosingBracketPos(tmp, "[]");

            if (pos == std::string::npos)
                throw JSONException("Array is never closed ']'\n" + beautify(tmp));

            value = tmp.substr(0, pos + 1);
        }
        else if (tmp.at(0) == '"')
        {
            // Get position of next quote
            pos = tmp.find('"', 1);

            if (pos == std::string::npos)
                throw JSONException("Expected '\"'\n" + beautify(tmp));

            value = tmp.substr(1, pos - 1);
        }
        else
            throw JSONException("Expected '[' or '\"'\n" + beautify(tmp));

        // Push key/values
        result.push_back(Pair(key, value));

        // Check that after a key/value there is a comma
        if ((tmp.length() - 1) == pos || tmp.at(pos + 1) != ',')
            throw JSONException("Expected ',' after key/value\n" + beautify(tmp));

        tmp = tmp.substr(pos + 1);
    }

    return (result);
}

std::vector<std::string> JSON::getValuesFromArray(const std::string &json)
{
    try
    {
        std::vector<std::string> result;

        std::string array = getContentFromArray(json);
        
        if (array.empty())
            return (result);

        while (true)
        {
            // if first character is a comma, skip it
            if (array.at(0) == ',')
                array = array.substr(1);

            // if after comma string is empty break because parsing is done
            if (array.empty())
                break;

            if (array.at(0) != '"')
                throw JSONException("Expected '\"'\n" + beautify(array));

            std::size_t pos = -1;

            int tmp = 0;
            while ((pos = array.find('"', pos + 1)) != std::string::npos)
            {
                tmp++;
                if (tmp > 1)
                    break;
            }

            if (pos == std::string::npos)
                throw JSONException("Expected value to end with '\"'\n" + beautify(array));

            std::string str = array.substr(1, pos - 1);

            if (str.empty())
                throw JSONException("Expected value between quotes, cannot have empty quotes in array\n" + beautify(array));

            // Push value to vector without quote
            result.push_back(str);

            // Check that after a value there is a comma
            if ((array.length() - 1) == pos || array.at(pos + 1) != ',')
                throw JSONException("Expected ',' after value\n" + beautify(array));

            array = array.substr(pos + 1);
        }

        return (result);
    }
    catch (std::exception &e)
    {
        throw;
    }
}

std::string JSON::sanitize(const std::string &json)
{
    std::string result = json;

    std::size_t pos;
    while ((pos = result.find_first_of(" \n")) != std::string::npos)
        result.erase(pos, 1);

    return (result);
}

// Add indentation and new line to sanitized JSON for easier error debugging
std::string JSON::beautify(const std::string &json)
{
    std::string result = json;

    int ob = 0;

    std::size_t pos = -1;
    while ((pos = result.find_first_of("[]{},:", pos + 1)) != std::string::npos)
    {
        if (result.at(pos) == ',')
        {
            if ((result.length() - 1) != pos && (result.at(pos + 1) == '}' || result.at(pos + 1) == ']'))
                ob--;

            result.insert(pos + 1, "\n");
            for (int i = 0; i < ob; i++)
                result.insert(pos + 2, "  ");
        }
        else if (result.at(pos) == ':')
            result.insert(pos + 1, " ");
        else if (result.at(pos) == '[' || result.at(pos) == '{')
        {
            ob++;
            result.insert(pos + 1, "\n");
            for (int i = 0; i < ob; i++)
                result.insert(pos + 2, "  ");
        }
    }

    return (result);
}

JSON::JSONException::JSONException(const std::string &message) : _message("JSON Error - " + message)
{
}

const char *JSON::JSONException::what() const throw()
{
    return (_message.c_str());
}