#include "Client.hpp"
#include "request/Response.hpp"

Client::Client(const int fd) : _fd(fd), _receive(true), _write("") {}

Client::~Client() {}

Client::Client(const Client &other) : _fd(other._fd), _receive(other._receive)
{
    _ossRead << other._ossRead.str();
}

void Client::onGetRequest(const Request &req, const Route *route)
{
    std::ostringstream path;

    if (req.getPath() == route->getRoute()) {
        path << route->getRoot() << "/" << route->getIndex();
    } else {
        path << route->getRoot() << "/" << req.getPath().substr(route->getRoute().size());
    }

    std::cout << "Path: " << path.str() << std::endl;

    try {
        Response res = Response::getFileResponse(path.str());
        _write = res.build();
    } catch (const std::exception &e) {
        Response res = Response::getErrorResponse(HttpStatusCode::NOT_FOUND, "data/404.html");
        _write = res.build();
    }
}

void Client::onFinishReceiving(const ServerConfig &config)
{
    std::string reqStr = _ossRead.str();

    if (config.getMaxBodySize() > (int) reqStr.length()) {
        Response res = Response::getErrorResponse(HttpStatusCode::BAD_REQUEST, "data/404.html");
        _write = res.build();
        return ;
    }

    Request req = Request::fromString(reqStr);

    //TODO: obligé d'utiliser compare sinon ça marche pas??
    if (req.getHttpVersion().compare("HTTP/1.1") == 0) {
        Response res = Response::getErrorResponse(HttpStatusCode::HTTP_VERSION_NOT_SUPPORTED, "data/404.html");
        _write = res.build();
        return ;
    }

    const Route *route = NULL;

    try {
        route = config.getRoute(req.getPath());
    } catch (const std::exception &e) {
        Response res = Response::getErrorResponse(HttpStatusCode::BAD_REQUEST, "data/404.html");
        _write = res.build();
        return ;
    }

    if (!route->isHTTPMethodAuthorized(req.getMethod())) {
        Response res = Response::getErrorResponse(HttpStatusCode::METHOD_NOT_ALLOWED, "data/404.html");
        _write = res.build();
        return ;
    }

    if (req.getMethod().getKey() == HttpMethod::GET.getKey()) {
        onGetRequest(req, route);
    }
}

bool Client::onReceive(void)
{
    if (!this->_receive) {
        throw ClientException("Input Stream Data is already stopped");
    }

    const int readed = read(_fd, _buffer, MAX_READ);

    if (readed <= 0) {
        throw ClientException("Input Stream Data was stopped");
    }

    _buffer[readed] = 0;

    _ossRead << _buffer;

    if (readed != MAX_READ) {
        this->_receive = false;
    }

    return (this->_receive);
}

bool Client::onSend(void)
{
    if (this->_write.empty()) {
        throw ClientException("Nothing to write");
    }

    std::string send;

    if (_write.size() > MAX_READ) {
        send = _write.substr(0, MAX_READ);
        _write = _write.substr(MAX_READ);
    } else {
        send = _write;
        _write = "";
    }

    if (write(_fd, send.c_str(), send.size()) <= 0) {
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

Client::ClientException::ClientException(const std::string &message) : _message("Server Error - " + message) {}

const char *Client::ClientException::what() const throw()
{
    return (_message.c_str());
}
