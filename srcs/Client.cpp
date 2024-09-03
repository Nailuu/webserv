#include "Client.hpp"
#include "request/Response.hpp"
#include <cstdio>
#include <iostream>
#include <string>

Client::Client(const int fd) : _fd(fd), _headerParsed(false), _receiving(true), _path(""), _contentLength(0), _reader(_fd), _write("") {}

Client::~Client() {}

Client::Client(const Client &other) : _fd(other._fd), _headerParsed(other._headerParsed), _receiving(other._receiving), _path(other._path), _contentLength(0), _reader(other._reader), _request(other._request), _write(other._write) {}

Client Client::operator=(const Client &other)
{
    if (this != &other)
    {
        _headerParsed = other._headerParsed;
        _receiving = other._receiving;
        _contentLength = other._contentLength;
        _path = other._path;
        _reader = other._reader;
        _request = other._request;
        _write = other._write;
    }
    return (*this);
}

void Client::onGetRequest(const Route *route)
{
    _receiving = false;

    try
    {
        Response res = Response::getFileResponse(_path, route->autoIndex(), route->getRoute());
        _write = res.build();
    }
    catch (const std::exception &e)
    {
        Response res = Response::getErrorResponse(HttpStatusCode::NOT_FOUND, "data/404.html");
        _write = res.build();
    }
}

void Client::onDeleteRequest()
{
    _receiving = false;

    std::ifstream file;
    file.open(_path.c_str());

    if (file.fail() || (file.close(), std::remove(_path.c_str())))
    {
        Response res = Response::getErrorResponse(HttpStatusCode::NOT_FOUND, "data/404.html");
        _write = res.build();
        return;
    }

    Response res = Response("HTTP/1.1", HttpStatusCode::OK);
    res.addField("Connection", "close");
    res.setContent("", MimeType::TEXT_PLAIN);
    _write = res.build();
}

void Client::onPostRequest()
{
    _receiving = false;

    std::cout << "Body: " << _reader.getBody() << std::endl;

    Response res = Response("HTTP/1.1", HttpStatusCode::OK);
    res.addField("Connection", "close");
    res.setContent("", MimeType::TEXT_PLAIN);
    _write = res.build();
}

bool Client::onHeaderReceived(const ServerConfig &config)
{
    _headerParsed = true;

    std::string header = _reader.getHeader();

    _request = Request::fromString(header);

    // TODO: obligé d'utiliser compare sinon ça marche pas??
    if (_request.getHttpVersion().compare("HTTP/1.1") == 0)
    {
        Response res = Response::getErrorResponse(HttpStatusCode::HTTP_VERSION_NOT_SUPPORTED, "data/404.html");
        _write = res.build();
        _receiving = false;
        return (false);
    }

    const Route *route = NULL;

    try
    {
        route = config.getRoute(_request.getPath());
    }
    catch (const std::exception &e)
    {
        Response res = Response::getErrorResponse(HttpStatusCode::BAD_REQUEST, "data/404.html");
        _write = res.build();
        _receiving = false;
        return (false);
    }

    if (!route->isHTTPMethodAuthorized(_request.getMethod()))
    {
        Response res = Response::getErrorResponse(HttpStatusCode::METHOD_NOT_ALLOWED, "data/404.html");
        _write = res.build();
        _receiving = false;
        return (false);
    }

    std::ostringstream path;
    std::ostringstream tempPath;

    if (_request.getPath().size() > 1 && _request.getPath().at(_request.getPath().size() - 1) == '/')
    {
        tempPath << _request.getPath().substr(0, _request.getPath().size() - 1);
    }
    else
    {
        tempPath << _request.getPath();
    }

    std::string pathStr = tempPath.str();

    path << route->getRoot();

    if (pathStr == route->getRoute())
    {
        path << "/" << route->getIndex();
    }
    else
    {
        pathStr = pathStr.substr(route->getRoute().size());
        if (pathStr.at(0) != '/')
        {
            path << "/";
        }
        path << pathStr;
    }

    _path = path.str();

    if (_request.getMethod().getKey() == HttpMethod::GET.getKey())
    {
        std::cout << GREEN << "[GET] " << GREY << _path << WHITE << std::endl;
        onGetRequest(route);
    }
    else if (_request.getMethod().getKey() == HttpMethod::DELETE.getKey())
    {
        std::cout << RED << "[DELETE] " << GREY << _path << WHITE << std::endl;
        onDeleteRequest();
    }
    else if (_request.getMethod().getKey() == HttpMethod::POST.getKey())
    {
        std::cout << YELLOW << "[POST] " << GREY << _path << WHITE << std::endl;

        std::map<std::string, std::string> fields = _request.getFields();
        std::map<std::string, std::string>::iterator it = fields.begin();

        for (; it != fields.end(); it++)
        {
            std::pair<std::string, std::string> pair = (*it);

            if (pair.first.compare("Content-Length") != 0)
                continue;

            std::istringstream iss(pair.second);

            if (iss >> _contentLength)
            {
                std::cout << "Content Length Value : " << _contentLength << std::endl;
                break;
            }
            else
                return (false);
        }

        if (_contentLength == std::string::npos)
            return (false);
    }
    return (true);
}

void Client::onReceive(const ServerConfig &config)
{
    try
    {
        _reader.onReceive(config);
    }
    catch (const std::exception &e)
    {
        throw ClientException("Cannot receive from client");
    }

    if (!_reader.isHeaderReceived())
        return;

    if (!_headerParsed)
    {
        if (!onHeaderReceived(config))
            throw ClientException("Error in Header");
    }

    if (_request.getMethod().getKey() != HttpMethod::POST.getKey())
        return;

    if (_reader.getBodySize() > _contentLength)
        throw ClientException("BodySize is too big !");

    if (_reader.getBodySize() == _contentLength)
        onPostRequest();
}

bool Client::onSend(void)
{
    if (this->_write.empty())
    {
        throw ClientException("Nothing to write");
    }

    std::string send;

    if (_write.size() > MAX_READ)
    {
        send = _write.substr(0, MAX_READ);
        _write = _write.substr(MAX_READ);
    }
    else
    {
        send = _write;
        _write = "";
    }

    if (write(_fd, send.c_str(), send.size()) <= 0)
    {
        throw ClientException("Output Stream Data was stopped");
    }

    return (!_write.empty());
}

bool Client::isReceiving(void) const
{
    return (this->_receiving);
}

void Client::onStop(void)
{
    _receiving = false;
    close(_fd);
}

Client::ClientException::ClientException(const std::string &message) : _message(std::string(RED) + "Client Error" + std::string(YELLOW) + " - " + message) {}

const char *Client::ClientException::what() const throw()
{
    return (_message.c_str());
}
