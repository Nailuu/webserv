#include "ServerConfig.hpp"

ServerConfig::ServerConfig(const std::vector<Pair> &pairs) : _max_body_size(DEFAULT_MAX_BODY_SIZE)
{
    // VectorDebugger<Pair>::print(pairs);

    std::string tmp;

    // PORT
    this->validate("port", pairs, tmp);
    this->stringToInt(tmp, this->_port, "port");

    // MAX BODY SIZE
    this->validate("max_body_size", pairs, tmp);
    this->stringToInt(tmp, this->_max_body_size, "max_body_size");

    // HOST
    this->validate("host", pairs, this->_host);

    // NAME
    this->validate("server_name", pairs, this->_name, false);

    // ROOT
    this->validate("root", pairs, this->_root);
    if (!this->isValidDirectory(this->_root))
        throw ServerConfigException("Path in 'root' is not a valid directory path: '" + this->_root + "'");

    // INDEX
    this->validate("index", pairs, this->_index);

    std::ifstream file;
    std::string path = this->_root + "/" + this->_index;

    file.open(path.c_str());

    if (file.fail())
        throw ServerConfigException("Can't open index file at '" + path + "': " + std::string(strerror(errno)));

    try
    {
        // HTTP METHODS
        this->validate("allowed_methods", pairs, tmp);

        std::vector<std::string> methods = JSON::getValuesFromArray(tmp);
        if (!methods.size())
            throw ServerConfigException("Expected at least one HTTP method in 'allowed_methods'");

        for (std::vector<std::string>::const_iterator it = methods.begin(); it != methods.end(); it++)
        {
            // VALIDATE HTTP METHODS
            // this->_accepted_http_methods.push_back();
        }

        // ROUTES
        // create default route on "/" based on default settings
        this->_routes.push_back(Route(this->_max_body_size, "/", this->_root, this->_index, this->_accepted_http_methods));

        // Create more routes if 'routes' is defined in the configuration JSON
        this->validate("routes", pairs, tmp, false);
        if (!tmp.empty())
        {
            std::vector<std::string> routes = JSON::getObjectsFromArray(tmp);
            for (std::vector<std::string>::const_iterator it = routes.begin(); it != routes.end(); it++)
            {
                // Initialize route with default settings
                Route r(this->_max_body_size, "unknown", this->_root, this->_index, this->_accepted_http_methods);

                // Overwrite default settings
                std::vector<Pair> p = JSON::getKeysAndValuesFromObject(*it);
                r.update(p);

                this->_routes.push_back(r);
            }
        }
    }
    catch (std::exception &e)
    {
        throw;
    }
}

ServerConfig::ServerConfig(const ServerConfig &sc)
{
    *this = sc;
}

ServerConfig &ServerConfig::operator=(const ServerConfig &sc)
{
    if (this != &sc)
    {
        this->_port = sc._port;
        this->_host = sc._host;
        this->_accepted_http_methods = sc._accepted_http_methods;
    }

    return (*this);
}

int ServerConfig::getPort() const
{
    return (this->_port);
}

int ServerConfig::getMaxBodySize() const
{
    return (this->_max_body_size);
}

const std::string &ServerConfig::getHost() const
{
    return (this->_host);
}

const std::string &ServerConfig::getName() const
{
    return (this->_name);
}

const std::string &ServerConfig::getRoot() const
{
    return (this->_root);
}

const std::string &ServerConfig::getIndex() const
{
    return (this->_index);
}

const std::vector<HTTP_METHOD> &ServerConfig::getHTTPMethods() const
{
    return (this->_accepted_http_methods);
}

const std::vector<Route> &ServerConfig::getRoutes() const
{
    return (this->_routes);
}

void ServerConfig::validate(const std::string &key, const std::vector<Pair> &pairs, std::string &result, bool mandatory)
{
    bool exist = Pair::exist(key, pairs);

    if (mandatory && !exist)
        throw ServerConfigException("No '" + key + "' defined in server configuration");
    else if (exist)
    {
        result = Pair::get(key, pairs).getValue();
        if (result.empty())
            throw ServerConfigException("Expected value in '" + key + "'");
    }
    else
        result = "";
}

bool ServerConfig::isValidDirectory(const std::string &path) const
{
    struct stat info;

    if (stat(path.c_str(), &info) != 0)
        return (false);

    if (info.st_mode & S_IFDIR)
        return (true);

    return (false);
}

void ServerConfig::stringToInt(const std::string &str, int &result, const std::string &context)
{
    std::stringstream ss(str);
    ss >> result;

    if (ss.fail() || !ss.eof())
        throw ServerConfigException("Value is not a valid int '" + str + "' in '" + context + "'");
}

ServerConfig::ServerConfigException::ServerConfigException(const std::string &message) : _message("Server Config Error - " + message)
{
}

const char *ServerConfig::ServerConfigException::what() const throw()
{
    return (_message.c_str());
}