#pragma once

#include <sstream>
#include <exception>
#include "config/ServerConfig.hpp"
#include "request/Request.hpp"

#define MAX_READ 1024

class Client
{
    private:
        char _buffer[MAX_READ + 1];
        const int _fd;
        bool _receive;
        std::ostringstream _ossRead;
        std::string _write;
        void onGetRequest(const Request &req, const Route *route);
    public:
        Client();
        Client(const int fd);
        ~Client();
        Client(const Client &other);
        Client operator=(const Client &other);
        bool onReceive(void);
        bool onSend(void);
        bool isReceiving(void);
        void onFinishReceiving(const ServerConfig &config);
        void onStop(void);
        class ClientException : public std::exception
        {
        public:
            ClientException(const std::string& message);
            virtual ~ClientException() throw() {};
            virtual const char* what() const throw();
        private:
            const std::string _message;
        };
};
