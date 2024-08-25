#include "global.hpp"

bool startsWith(const std::string &str, const std::string &prefix)
{
    return (str.compare(0, prefix.size(), prefix) == 0);
}

HTTP_METHOD getHttpMethodFromString(const std::string &str)
{
    for (unsigned int i = 0; i < sizeof(HTTP_METHODS) / sizeof(char *); i++)
    {
        if (str == std::string(HTTP_METHODS[i]))
            return ((HTTP_METHOD)i);
    }

    return (UNKNOWN);
}

std::string getStringFromHttpMethod(HTTP_METHOD method)
{
    switch (method)
    {
    case GET:
        return ("GET");
    case POST:
        return ("POST");
    case PUT:
        return ("PUT");
    case DELETE:
        return ("DELETE");
    default:
        return ("");
    }
}

bool isValidDirectory(const std::string &path)
{
    struct stat info;

    if (stat(path.c_str(), &info) != 0)
        return (false);

    if (info.st_mode & S_IFDIR)
        return (true);

    return (false);
}

std::string GetStatusCodeMessage(HttpStatusCode code)
{
    switch (code)
    {
    case Continue:
        return "Continue";
    case SwitchingProtocols:
        return "Switching Protocols";
    case Processing:
        return "Processing";

    case OK:
        return "OK";
    case Created:
        return "Created";
    case Accepted:
        return "Accepted";
    case NonAuthoritativeInformation:
        return "Non-Authoritative Information";
    case NoContent:
        return "No Content";
    case ResetContent:
        return "Reset Content";
    case PartialContent:
        return "Partial Content";

    case MultipleChoices:
        return "Multiple Choices";
    case MovedPermanently:
        return "Moved Permanently";
    case Found:
        return "Found";
    case SeeOther:
        return "See Other";
    case NotModified:
        return "Not Modified";
    case TemporaryRedirect:
        return "Temporary Redirect";
    case PermanentRedirect:
        return "Permanent Redirect";

    case BadRequest:
        return "Bad Request";
    case Unauthorized:
        return "Unauthorized";
    case PaymentRequired:
        return "Payment Required";
    case Forbidden:
        return "Forbidden";
    case NotFound:
        return "Not Found";
    case MethodNotAllowed:
        return "Method Not Allowed";
    case NotAcceptable:
        return "Not Acceptable";
    case ProxyAuthenticationRequired:
        return "Proxy Authentication Required";
    case RequestTimeout:
        return "Request Timeout";
    case Conflict:
        return "Conflict";

    case InternalServerError:
        return "Internal Server Error";
    case NotImplemented:
        return "Not Implemented";
    case BadGateway:
        return "Bad Gateway";
    case ServiceUnavailable:
        return "Service Unavailable";
    case GatewayTimeout:
        return "Gateway Timeout";
    case HTTPVersionNotSupported:
        return "HTTP Version Not Supported";

    default:
        return "Unknown Status Code";
    }
}
