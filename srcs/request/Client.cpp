#include "Client.hpp"
#include "Response.hpp"

#include <cstdio>
#include <iostream>
#include <string>

Client::Client(const int fd) : _fd(fd), _headerParsed(false), _receiving(true), _path(""), _contentLength(std::string::npos), _reader(_fd), _write("") {}

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

void Client::onGetRequest(bool autoIndex)
{
    _receiving = false;

    try
    {
        Response res = Response::getFileResponse(_path, autoIndex, _request.getPath());
        _write = res.build(_request);
    }
    catch (const std::exception &e)
    {
        std::cerr << RED << "Runtime Error: " << e.what() << WHITE << std::endl;

        Response res = Response::getErrorResponse(HttpStatusCode::NOT_FOUND);
        _write = res.build(_request);
    }
}

void Client::onDeleteRequest()
{
    _receiving = false;

    std::ifstream file;
    file.open(_path.c_str());

    if (file.fail() || (file.close(), std::remove(_path.c_str())))
    {
        Response res = Response::getErrorResponse(HttpStatusCode::NOT_FOUND);
        _write = res.build(_request);
        return;
    }

    Response res = Response("HTTP/1.1", HttpStatusCode::OK);
    res.addField("Connection", "close");
    res.setContent("", MimeType::TEXT_PLAIN);
    _write = res.build(_request);
}

void Client::onPostRequest()
{
    _receiving = false;

    std::string body = _reader.getBody();

    if (_contentLength != std::string::npos)
    {
        body = body.substr(_contentLength);
    }
    else
    {
        body = body.substr(0, body.length() - 4); // Remove "\r\n\r\n"
    }

    Response res = Response("HTTP/1.1", HttpStatusCode::OK);
    res.addField("Connection", "close");
    res.setContent("", MimeType::TEXT_PLAIN);
    _write = res.build(_request);
}

bool Client::HandleRequest(const ServerConfig &config)
{
    if (_request.getHttpVersion() != "HTTP/1.1")
    {
        Response res = Response::getErrorResponse(HttpStatusCode::HTTP_VERSION_NOT_SUPPORTED);
        _write = res.build(_request);
        _receiving = false;
        return (true);
    }

    const Route *route = NULL;

    try
    {
        route = config.getRoute(_request.getPath());
    }
    catch (const std::exception &e)
    {
        Response res = Response::getErrorResponse(HttpStatusCode::BAD_REQUEST);
        _write = res.build(_request);
        _receiving = false;
        return (true);
    }

    if (!route->isHTTPMethodAuthorized(_request.getMethod()))
    {
        // std::cout << "Method not allowed!" << std::endl;
        Response res = Response::getErrorResponse(HttpStatusCode::METHOD_NOT_ALLOWED);
        _write = res.build(_request);
        _receiving = false;
        return (true);
    }

    bool isCGI = false;
    if (route->isCGI())
    {
        std::string tmp = _request.getPath();

        std::size_t pos = tmp.find_last_of('.');
        if (pos != std::string::npos)
        {
            if (tmp.substr(pos) == route->getExtension() && pos != 0 && tmp.at(pos - 1) != '/')
                isCGI = true;
        }
    }

    // If redirection is not empty, there is a redirection
    if (!isCGI && !route->getRedirection().empty())
    {
        // Create redirection response
        Response res("HTTP/1.1", HttpStatusCode::MOVED_PERMANENTLY);
        res.addField("Connection", "close");

        // Most important part
        res.addField("Location", route->getRedirection());

        _write = res.build(_request);
        _receiving = false;
        return (true);
    }

    std::ostringstream path;
    std::ostringstream tempPath;

    bool directory = false;

    if (_request.getPath().size() > 1 && _request.getPath().at(_request.getPath().size() - 1) == '/')
    {
        tempPath << _request.getPath().substr(0, _request.getPath().size() - 1);
        directory = true;
    }
    else
    {
        tempPath << _request.getPath();
    }

    std::string pathStr = tempPath.str();

    path << route->getRoot();

    // Fix : Enable auto index only if path is equal to route
    bool autoIndex = false;

    // Fix : Returns index page if path starts with route and points a folder (ex: route = /directory, path = /directory/nop)
    if (pathStr == route->getRoute())
    {
        // If autoindex is enable, path = directory
        autoIndex = true;
        path << "/" << (route->autoIndex() ? "" : route->getIndex());
    }
    else
    {
        // Test if points a directory
        struct stat statBuf;

        std::string test_path = path.str() + "/" + pathStr.substr(route->getRoute().length());

        // If points a directory, redirects to index
        if (stat(test_path.c_str(), &statBuf) == 0 && S_ISDIR(statBuf.st_mode))
        {
            path << pathStr.substr(route->getRoute().length()) << "/" << route->getIndex();
        }
        else
        {
            // EXAMPLE: Route -> /media, Root: /data/uploads/
            // Request = /media/file.png
            // Alias: /data/uploads/file.png
            // Not alias: /data/uploads/media/file.png
            if (route->isAlias())
            {
                pathStr = pathStr.substr(route->getRoute().size());
                if (pathStr.at(0) != '/')
                    path << "/";
            }

            path << pathStr << (directory ? "/" : "");
        }
    }

    _path = path.str();

    if (isCGI)
    {
        try
        {
            CGIHandler handler;

            // Init CGI environment variables
            handler.init(config, this->_request, route, _path);

            // Execute CGI script
            bool timedout = false;
            const std::string output = handler.execute(timedout);

            _receiving = false;

            // If CGI script was still runing after TIMEOUT seconds
            if (timedout)
            {
                Response res("HTTP/1.1", HttpStatusCode::GATEWAY_TIMEOUT);
                res.setContent("<h1 style=\"text-align: center\">CGI SCRIPT TIMEOUT</h1>", MimeType::TEXT_HTML);
                _write = res.build(_request);

                return (true);
            }

            Response res("HTTP/1.1", HttpStatusCode::OK);
            res.setContent(output, MimeType::TEXT_HTML);
            res.addField("Connection", "close");

            _write = res.build(_request);
        }
        catch (const std::exception &e)
        {
            std::string error = e.what();

            if (!startsWith(error, "---\n"))
                std::cerr << RED << "CGI Runtime Error: CGIHandler Error " << YELLOW << " - " << e.what() << WHITE << std::endl;

            _receiving = false;

            Response res = Response::getErrorResponse(HttpStatusCode::INTERNAL_SERVER_ERROR);

            if (startsWith(error, "---\n"))
            {
                std::ostringstream content;
                content << "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>CGI Script Error</title><style>body{font-family:Arial,sans-serif;background-color:#f8f8f8;color:#333;text-align:center;margin:50px;}.error-box{background-color:#eaeaea;color:#d95050;text-align:start;padding:20px;border-radius:5px;display:inline-block;white-space:pre-wrap;}</style></head><body><h1>CGI Script Error</h1><div class=\"error-box\">";
                content << error.substr(5);
                content << "</div></body></html>";

                res.setContent(content.str(), MimeType::TEXT_HTML);
            }

            _write = res.build(_request);

            return (true);
        }
    }
    else if (_request.getMethod().getKey() == HttpMethod::GET.getKey())
    {
        onGetRequest(autoIndex);
    }
    else if (_request.getMethod().getKey() == HttpMethod::DELETE.getKey())
    {
        onDeleteRequest();
    }

    return (true);
}

bool Client::onHeaderReceived(const ServerConfig &config)
{
    _headerParsed = true;

    std::string header = _reader.getHeader();

    // std::cout << header << std::endl;

    try
    {
        _request = Request::fromString(header);
    }
    catch (const std::exception &e)
    {
        const std::string cause = std::string(e.what());

        if (cause.find("Invalid Method") != std::string::npos)
        {
            Response res = Response::getErrorResponse(HttpStatusCode::METHOD_NOT_ALLOWED);

            std::string method = cause.substr(cause.find('\'') + 1);
            method.erase(method.size() - 1);

            _write = res.build(_request, method);
            _receiving = false;
            return (true);
        }
        throw ClientException("Invalid Request Header : " + cause);
    }

    // If request is POST, only do checks after whole content is received

    if (_request.getMethod().getKey() == HttpMethod::POST.getKey())
    {
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
                // std::cout << "Content Length Value : " << _contentLength << std::endl;
                if (_contentLength == 0)
                {
                    Response res = Response::getErrorResponse(HttpStatusCode::BAD_REQUEST);
                    _write = res.build(_request);
                    _receiving = false;
                    return (true);
                }
                else if (_contentLength > (size_t)config.getMaxBodySize())
                {
                    Response res = Response::getErrorResponse(HttpStatusCode::PAYLOAD_TOO_LARGE);
                    _write = res.build(_request);
                    _receiving = false;
                    return (true);
                }
                break;
            }

            break;
        }
    }

    return (HandleRequest(config));
}

void Client::onReceive(const ServerConfig &config)
{
    if (!_receiving)
        return;

    try
    {
        _reader.onReceive(config);
    }
    catch (const std::exception &e)
    {
        std::string cause = std::string(e.what());

        // Body Size too big
        if (cause.find("Body size exceeds") != std::string::npos)
        {
            Response res = Response::getErrorResponse(HttpStatusCode::PAYLOAD_TOO_LARGE);
            _write = res.build(_request);
            _receiving = false;
            return;
        }
        else if (cause.find("Header size exceeds") != std::string::npos)
        {
            Response res = Response::getErrorResponse(HttpStatusCode::REQUEST_HEADER_FIELDS_TOO_LARGE);
            _write = res.build(_request);
            _receiving = false;
            return;
        }

        throw ClientException("Cannot receive from client: '" + std::string(e.what()) + "'");
    }

    if (!_reader.isHeaderReceived())
        return;

    if (!_headerParsed)
    {
        if (!onHeaderReceived(config))
            throw ClientException("Error in Header");
    }

    if (_receiving == false || _request.getMethod().getKey() != HttpMethod::POST.getKey())
        return;

    if (_reader.getBodySize() > _contentLength)
    {
        Response res = Response::getErrorResponse(HttpStatusCode::PAYLOAD_TOO_LARGE);
        _write = res.build(_request);
        _receiving = false;
        return;
    }

    // If contentLength is present
    if (_contentLength != std::string::npos)
    {
        if (_reader.getBodySize() >= _contentLength)
            HandleRequest(config);
    }
    else
    {
        // Check if end of content is \r\n\r\n

        std::string body = _reader.getBody();

        size_t pos = body.find("\r\n\r\n");

        if (pos != std::string::npos && pos + 4 == body.length())
            onPostRequest();
    }
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
