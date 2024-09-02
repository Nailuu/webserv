#include "Client.hpp"
#include "request/Response.hpp"
#include <cstdio>

Client::Client(const int fd) : _fd(fd), _receive(true), _dataIndex(0), _dataRead(0), _write("") {}

Client::~Client() {}

Client::Client(const Client &other) : _fd(other._fd), _receive(other._receive), _dataIndex(other._dataIndex), _dataRead(other._dataRead), _write(other._write)
{
    _ossRead << other._ossRead.str();
}

void Client::onGetRequest(const Request &req, const std::string &path, const Route *route)
{
    (void)req;
    this->_receive = false;

    try
    {
        Response res = Response::getFileResponse(path, route->autoIndex(), route->getRoute());
        _write = res.build();
    }
    catch (const std::exception &e)
    {
        Response res = Response::getErrorResponse(HttpStatusCode::NOT_FOUND, "data/404.html");
        _write = res.build();
    }
}

void Client::onDeleteRequest(const Request &req, const std::string &path)
{
    (void)req;
    this->_receive = false;

    std::ifstream file;
    file.open(path.c_str());

    if (file.fail() || (file.close(), remove(path.c_str())))
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

void Client::onFinishReceiving(const ServerConfig &config)
{
    std::string reqStr = _ossRead.str();

    if (config.getMaxBodySize() > _dataIndex)
    {
        Response res = Response::getErrorResponse(HttpStatusCode::BAD_REQUEST, "data/404.html");
        _write = res.build();
        return;
    }

    _request = Request::fromString(reqStr);

    // TODO: obligé d'utiliser compare sinon ça marche pas??
    if (_request.getHttpVersion().compare("HTTP/1.1") == 0)
    {
        Response res = Response::getErrorResponse(HttpStatusCode::HTTP_VERSION_NOT_SUPPORTED, "data/404.html");
        _write = res.build();
        return;
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
        return;
    }

    if (!route->isHTTPMethodAuthorized(_request.getMethod()))
    {
        Response res = Response::getErrorResponse(HttpStatusCode::METHOD_NOT_ALLOWED, "data/404.html");
        _write = res.build();
        return;
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

    std::cout << "Path: " << path.str() << std::endl;

    if (_request.getMethod().getKey() == HttpMethod::GET.getKey())
    {
        onGetRequest(_request, path.str(), route);
    }
    else if (_request.getMethod().getKey() == HttpMethod::DELETE.getKey())
    {
        onDeleteRequest(_request, path.str());
    }
}

bool Client::onReceive(void)
{
    if (!this->_receive)
    {
        throw ClientException("Input Stream Data is already stopped");
    }

    const int readed = read(_fd, _buffer, MAX_READ);

    if (readed <= 0)
    {
        throw ClientException("Input Stream Data was stopped");
    }

    _buffer[readed] = 0;

    bool hasReachedHeaderEnd = false;

    _ossRead << _buffer;

    if (_dataIndex == 0)
    {
        size_t headerEnd = _ossRead.str().find("\r\n\r\n");

        if (headerEnd != std::string::npos)
        {
            _dataIndex = headerEnd + 4;
        }

        hasReachedHeaderEnd = true;

        _dataRead = readed - _dataIndex;
    }
    else
    {
        _dataRead += readed;

        if (_request.getMethod().getKey() == HttpMethod::POST.getKey())
        {
            std::map<std::string, std::string>::const_iterator it = _request.getFields().find("Content-Length");

            if (it == _request.getFields().end())
            {
                throw ClientException("Content-Length is needed for POST");
            }

            std::string contentLength = (*it).second;

            int length = atoi(contentLength.c_str());

            if (_dataRead > length)
            {
                throw ClientException("Data too huge");
            }
            else if (_dataRead == length)
            {
                this->_receive = false;
            }
        }
    }

    if (readed != MAX_READ)
    {
        this->_receive = false;
    }

    return (!hasReachedHeaderEnd);
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

bool Client::isReceiving(void)
{
    return (this->_receive);
}

void Client::onStop(void)
{
    close(_fd);
}

Client::ClientException::ClientException(const std::string &message) : _message(std::string(RED) + "Client Error" + std::string(YELLOW) + " - " + message) {}

const char *Client::ClientException::what() const throw()
{
    return (_message.c_str());
}
