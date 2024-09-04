#include "Response.hpp"

Response::Response(const std::string &httpVersion, const HttpStatusCode &statusCode) : HTTPPayload(httpVersion), _statusCode(statusCode) {}

Response::Response(const Response &other) : HTTPPayload(other), _statusCode(other._statusCode), _content(other._content), _mimeType(other._mimeType) {}

Response Response::operator=(const Response &other)
{
    if (this != &other)
    {
        _content = other._content;
        _mimeType = other._mimeType;
    }
    return (*this);
}

const std::string Response::getCurrentTimeInHttpFormat() const
{
    std::time_t now = std::time(NULL);

    std::tm *gmtTime = std::gmtime(&now);

    char buffer[100];

    std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmtTime);

    return std::string(buffer);
}

void Response::setContent(const std::string &content, const MimeType &mimeType)
{
    _content = content;
    _mimeType = mimeType;
}

void Response::setContentFile(const std::string &path)
{
    std::size_t index = path.rfind(".");

    if (index == std::string::npos)
    {
        throw HTTPPayloadException("Target file does not have extension !");
    }

    std::string extension = path.substr(index + 1);

    try
    {
        _mimeType = MimeType::getByExtension(extension);
    }
    catch (std::runtime_error &error)
    {
        throw HTTPPayloadException(error.what());
    }

    std::ifstream file(path.c_str());

    if (!file.is_open())
    {
        throw HTTPPayloadException("Cannot open target file: '" + highlight(path) + "'");
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    _content = buffer.str();

    file.close();
}

const HttpStatusCode &Response::getStatusCode(void) const
{
    return (_statusCode);
}

const Response Response::getFileResponse(const std::string &path, bool autoindex, const std::string &route)
{
    Response res("HTTP/1.1", HttpStatusCode::OK);

    // If autoindex is enabled and the path is a directory (ends with '/') previous check in Client.cpp
    if (autoindex && path.at(path.size() - 1) == '/')
    {
        try
        {
            const std::string html = AutoIndexGenerator::generate(path, route);
            MimeType mime = MimeType::getByExtension("html");

            res.setContent(html, mime);
        }
        catch (std::exception &e)
        {
            throw;
        }
    }
    else
        res.setContentFile(path);

    res.addField("Connection", "close");

    return (res);
}

const Response Response::getErrorResponse(const HttpStatusCode &status)
{
    Response res("HTTP/1.1", status);

    // If error code has a file path set from configuration file, import the path
    if (status.isSet())
        res.setContentFile(status.getPath());

    res.addField("Connection", "close");

    return (res);
}

const std::string Response::build(const Request &req) const
{
    const std::string method = req.getMethod().getValue();

    std::string color;
    if (method == "GET")
        color = GREEN;
    else if (method == "DELETE")
        color = RED;
    else if (method == "POST" || method == "PUT")
        color = YELLOW;

    std::cout << color << "[" << method << "] " << GREY << req.getPath() << " - " << (_statusCode.getValue() == 200 ? BLUE : YELLOW) << _statusCode.getValue() << " " << _statusCode.getKey() << WHITE << std::endl;

    std::ostringstream oss;

    oss << _httpVersion << " " << _statusCode.getValue() << " " << _statusCode.getKey() << "\r" << std::endl;

    // Adding mandatory headers
    oss << "Date: " << getCurrentTimeInHttpFormat() << "\r" << std::endl;
    oss << "Server: " << SERVER_NAME << "\r" << std::endl;
    oss << "Content-Length: " << _content.length() << "\r" << std::endl;

    // TODO: Need to be implement
    oss << "Content-Type: " << _mimeType.getValue() << "\r" << std::endl;

    std::map<std::string, std::string>::const_iterator it = _fields.begin();

    for (; it != _fields.end(); it++)
    {
        std::pair<std::string, std::string> info = *it;

        oss << info.first << ": " << info.second << "\r" << std::endl;
    }

    oss << "\r" << std::endl
        << _content;
    return (oss.str());
}
