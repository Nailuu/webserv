#pragma once

#include <map>
#include <vector>
#include <string>
#include <exception>

// Forward declare the template function
template<typename T>
class Enum;

// Declare the friend function as a template
template<typename T>
std::ostream &operator<<(std::ostream &os, const Enum<T> &e);

template <typename T>
class Enum
{
   protected:
        std::string _key;
        T _value;
    public:
        Enum();
        Enum(const std::string &key, const T &value);
        virtual ~Enum() = 0;
        Enum(const Enum &other);
        const std::string &getKey(void) const;
        const T &getValue(void) const;
        // Exception class
        class EnumException : public std::exception
        {
        public:
            EnumException(const std::string &message);
            virtual ~EnumException() throw() {};
            virtual const char *what() const throw();

        private:
            const std::string _message;
        };
};

#include "Enum.tpp"
