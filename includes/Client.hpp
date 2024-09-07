#pragma once

#include <sstream>
#include <exception>

#include "ServerConfig.hpp"
#include "Request.hpp"
#include "StreamReader.hpp"
#include "CGIHandler.hpp"

#define MAX_READ 1024

class Client
{
    private:
        const int _fd;
        bool _headerParsed;
        bool _receiving;
        std::string _path;
        size_t _contentLength;
        StreamReader _reader;
        Request _request;
        std::string _write;
        CGIHandler _handler;
        bool HandleRequest(const ServerConfig &config);
        bool onHeaderReceived(const ServerConfig &config);
        void onGetRequest(bool autoIndex);
        void onDeleteRequest(void);
        void onPostRequest(void);

    public:
        Client();
        Client(const int fd);
        ~Client();
        Client(const Client &other);
        Client operator=(const Client &other);
        void onReceive(const ServerConfig &config);
        bool onSend(void);
        bool isReceiving(void) const;
        void onStop(void);
        CGIHandler& getCGIHandler(void);

        // Exception class
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
