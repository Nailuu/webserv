#pragma once

#include <sstream>
#include <exception>

#include "ServerConfig.hpp"
#include "Request.hpp"
#include "StreamReader.hpp"
#include "CGIHandler.hpp"
#include "CookiesManager.hpp"

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
    CookiesManager &_cookies;
    bool HandleRequest(const ServerConfig &config, ServerManager *manager);
    bool onHeaderReceived(const ServerConfig &config, ServerManager *manager);
    void onGetRequest(bool autoIndex);
    void onDeleteRequest(void);
    void onPostRequest(void);

public:
    Client();
    Client(const int fd, CookiesManager &cookiesManager);
    ~Client();
    Client(const Client &other);
    Client operator=(const Client &other);
    void onReceive(const ServerConfig &config, ServerManager *manager);
    bool onSend(void);
    bool isReceiving(void) const;
    void onStop(void);
    CGIHandler &getCGIHandler(void);

    // Exception class
    class ClientException : public std::exception
    {
    public:
        ClientException(const std::string &message);
        virtual ~ClientException() throw() {};
        virtual const char *what() const throw();

    private:
        const std::string _message;
    };
};
