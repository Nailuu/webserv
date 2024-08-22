#pragma once

#include <vector>
#include <utility>
#include <string>
#include <exception>

typedef std::vector<std::pair<std::string, std::string> > Pairs;

class JSON
{
public:
    static Pairs GetKeysAndValuesFromObject(const std::string& json);
    static std::vector<std::string> GetObjectsFromArray(const std::string& json);

    // Exception class
    class JSONException : public std::exception
    {
    public:
        JSONException(const std::string& message);
        virtual ~JSONException() throw() {};
        virtual const char* what() const throw();
    private:
        const std::string _message;
    };
};