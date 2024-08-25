#pragma once

#include <vector>
#include <utility>
#include <string>
#include <exception>

#include "Pair.hpp"

class JSON
{
public:
    virtual ~JSON() = 0;
    static std::vector<Pair> getKeysAndValuesFromObject(const std::string &json);
    static std::vector<std::string> getObjectsFromArray(const std::string &json);
    static std::vector<std::string> getValuesFromArray(const std::string &json);
    static std::string sanitize(const std::string &json);
    static std::string beautify(const std::string &json);

private:
    static std::string getContentFromArray(const std::string &json);
    static std::size_t getClosingBracketPos(const std::string &json, std::string bracket_type);

    // Exception class
    class JSONException : public std::exception
    {
    public:
        JSONException(const std::string &message);
        virtual ~JSONException() throw() {};
        virtual const char *what() const throw();

    private:
        const std::string _message;
    };
};