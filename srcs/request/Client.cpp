#include "Client.hpp"
#include "Response.hpp"

#include <cstdio>
#include <iostream>
#include <string>
#include <fstream>

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

    // Verify path points a dir
    struct stat statBuf;

    if (stat(_path.c_str(), &statBuf) != 0 || !S_ISDIR(statBuf.st_mode))
    {
        Response res = Response::getErrorResponse(HttpStatusCode::BAD_REQUEST);
        _write = res.build(_request);
        return;
    }

    std::string body = _reader.getBody();

    if (_contentLength != std::string::npos)
    {
        body = body.substr(0, _contentLength);
    }
    else
    {
        body = body.substr(0, body.length() - 4); // Remove "\r\n\r\n"
    }

    std::map<std::string, std::string>::const_iterator it = _request.getFields().find("Content-Type");
    std::string boundary;

    // Find Boundary
    if (it != _request.getFields().end())
    {
        std::string value = (*it).second;

        std::string multipartBeginValue = "multipart/form-data; boundary=";

        if (value.find(multipartBeginValue) == 0)
        {
            boundary = value.substr(multipartBeginValue.length());

            if (boundary.at(boundary.length() - 1) == '\r')
            {
                boundary = boundary.substr(0, boundary.length() - 1);
            }
        }
    }

    std::string filename;
    bool success = false;

    // Get files infos
    if (!boundary.empty())
    {
        // Check body starts with --boundary
        if (body.find("--" + boundary) != 0)
        {
            goto end;
        }

        size_t endLine = body.find("\n");

        if (endLine == std::string::npos)
        {
            goto end;
        }

        body = body.substr(endLine + 1);

        // Get Content-Disposition Header (mandatory)

        std::string header = "Content-Disposition: form-data";

        size_t headerIndex = body.find(header);

        if (headerIndex == std::string::npos)
        {
            goto end;
        }

        body = body.substr(headerIndex + header.length());

        // Verify name exists
        size_t nameIndex = body.find("name=\"");

        if (nameIndex == std::string::npos || body.substr(nameIndex + 6).find("\"") == std::string::npos)
        {
            goto end;
        }

        // Verify filename exists
        size_t filenameIndex = body.find("filename=\"");

        if (filenameIndex == std::string::npos)
        {
            goto end;
        }

        body = body.substr(filenameIndex + 10);

        // Get fileName
        size_t filenameEndIndex = body.find("\"");

        if (filenameEndIndex == std::string::npos)
        {
            goto end;
        }

        filename = body.substr(0, filenameEndIndex);

        body = body.substr(filenameEndIndex + 2);

        // Pass every other headers
        std::string endData = "\r\n\r\n";

        size_t dataBeginIndex = body.find(endData);

        if (dataBeginIndex == std::string::npos)
        {
            endData = "\n\n";
            dataBeginIndex = body.find(endData);
            if (dataBeginIndex == std::string::npos)
            {
                goto end;
            }
        }

        body = body.substr(dataBeginIndex + endData.length());

        // Get end of content
        size_t endContentIndex = body.find("\n--" + boundary + "--");

        if (endContentIndex == std::string::npos)
        {
            goto end;
        }

        body = body.substr(0, endContentIndex);
        success = true;
    }

end:

    // Body is equals to file Content
    if (success)
    {
        std::ostringstream filePath;

        filePath << _path;

        if (_path.at(_path.length() - 1) != '/')
        {
            filePath << '/';
        }

        filePath << filename;

        std::ofstream file(filePath.str().c_str(), std::ios::out | std::ios::app);

        if (!file.is_open())
        {
            Response res = Response::getErrorResponse(HttpStatusCode::FORBIDDEN);
            _write = res.build(_request);
            return;
        }

        file << body;

        file.close();

        Response res = Response("HTTP/1.1", HttpStatusCode::OK);
        res.addField("Connection", "close");
        res.setContent("", MimeType::TEXT_HTML);
        _write = res.build(_request);
        return;
    }

    Response res = Response::getErrorResponse(HttpStatusCode::BAD_REQUEST);
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
    int CGIIndex = 0;

    if (route->isCGI())
    {
        std::string tmp = _request.getPath();

        std::size_t pos = tmp.find_last_of('.');
        if (pos != std::string::npos)
        {
            const std::vector<std::string> ext = route->getExtensions();

            int index = 0;
            for (std::vector<std::string>::const_iterator it = ext.begin(); it != ext.end(); it++)
            {
                if (tmp.substr(pos) == (*it) && pos != 0 && tmp.at(pos - 1) != '/')
                {
                    isCGI = true;
                    CGIIndex = index;
                }

                index++;
            }
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
            if (!this->_handler.isActive())
            {
                // Init CGI environment variables
                this->_handler.init(config, this->_request, route, _path, CGIIndex);

                // Execute CGI script
                this->_handler.execute();
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << RED << "CGI Runtime Error: CGIHandler Error " << YELLOW << " - " << e.what() << WHITE << std::endl;

            Response res = Response::getErrorResponse(HttpStatusCode::INTERNAL_SERVER_ERROR);
            _write = res.build(_request);
        }

        _receiving = false;
        return (true);
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
            onPostRequest();
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
    try
    {
        if (this->_handler.isActive())
        {

            // Non blocking waitpid, flag -> WNOHANG
            pid_t result = waitpid(this->_handler.getPid(), NULL, WNOHANG);

            // Fork still running
            if (result == 0)
                return (true);

            // Fork has stopped
            else if (result > 0)
            {
                this->_handler.readPipes();

                // If fork exited with code 1
                if (this->_handler.hasErrors())
                {
                    Response res = Response::getErrorResponse(HttpStatusCode::INTERNAL_SERVER_ERROR);

                    std::ostringstream content;
                    content << "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>CGI Script Error</title><style>body{font-family:Arial,sans-serif;background-color:#f8f8f8;color:#333;text-align:center;margin:50px;}.error-box{background-color:#eaeaea;color:#d95050;text-align:start;padding:20px;border-radius:5px;display:inline-block;white-space:pre-wrap;}</style></head><body><h1>CGI Script Error</h1><div class=\"error-box\">";
                    content << this->_handler.getErrors();
                    content << "</div></body></html>";

                    res.setContent(content.str(), MimeType::TEXT_HTML);

                    this->_write = res.build(_request);
                }
                else
                {
                    // Fork execution has timedout
                    if (this->_handler.hasTimedOut())
                    {
                        Response res("HTTP/1.1", HttpStatusCode::GATEWAY_TIMEOUT);
                        res.setContent("<h1 style=\"text-align: center\">CGI SCRIPT TIMEOUT</h1>", MimeType::TEXT_HTML);

                        this->_write = res.build(_request);
                    }
                    else
                    {
                        Response res("HTTP/1.1", HttpStatusCode::OK);
                        res.setContent(this->_handler.getOutput(), MimeType::TEXT_HTML);
                        res.addField("Connection", "close");

                        this->_write = res.build(_request);
                    }
                }
            }
            else
                throw ClientException("Waitpid of CGI fork has failed!");
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << RED << "Runtime Error: CGI/Client Error " << YELLOW << " - " << e.what() << WHITE << std::endl;

        Response res = Response::getErrorResponse(HttpStatusCode::INTERNAL_SERVER_ERROR);
        _write = res.build(_request);
    }

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

CGIHandler &Client::getCGIHandler(void)
{
    return (this->_handler);
}

Client::ClientException::ClientException(const std::string &message) : _message(std::string(RED) + "Client Error" + std::string(YELLOW) + " - " + message) {}

const char *Client::ClientException::what() const throw()
{
    return (_message.c_str());
}
