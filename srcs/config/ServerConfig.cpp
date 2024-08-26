#include "ServerConfig.hpp"

ServerConfig::ServerConfig() : _max_body_size(DEFAULT_MAX_BODY_SIZE) {}

void ServerConfig::build(const std::vector<Pair> &pairs)
{
    std::string tmp;

    // PORT
    this->validate("port", pairs, tmp);
    this->stringToInt(tmp, this->_port, "port");

    // MAX BODY SIZE
    this->validate("max_body_size", pairs, tmp, false);
    if (!tmp.empty())
        this->stringToInt(tmp, this->_max_body_size, "max_body_size");

    // HOST
    this->validate("host", pairs, this->_host);

    // NAME
    this->validate("server_name", pairs, this->_name, false);

    // ROOT
    this->validate("root", pairs, this->_root);
    if (!isValidDirectory(this->_root))
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
            HttpMethod hm;
            
            try {
                hm = HttpMethod::get(*it);
            } catch (const HttpMethod::EnumException &e) {
                throw ServerConfigException("Invalid HTTP Method in 'allowed_methods': '" + *it + "'");
            }

            this->_accepted_http_methods.push_back(hm);
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

int ServerConfig::getPort(void) const
{
    return (this->_port);
}

int ServerConfig::getMaxBodySize(void) const
{
    return (this->_max_body_size);
}

const std::string &ServerConfig::getHost(void) const
{
    return (this->_host);
}

const std::string &ServerConfig::getName(void) const
{
    return (this->_name);
}

const std::string &ServerConfig::getRoot(void) const
{
    return (this->_root);
}

const std::string &ServerConfig::getIndex(void) const
{
    return (this->_index);
}

const std::vector<HttpMethod> &ServerConfig::getHTTPMethods(void) const
{
    return (this->_accepted_http_methods);
}

const std::vector<Route> &ServerConfig::getRoutes(void) const
{
    return (this->_routes);
}

const Route *ServerConfig::getRoute(const std::string &path) const
{
    if (_routes.empty()) {
        throw ServerConfigException("No route was registered");
    }

    std::vector<Route>::const_iterator it = _routes.begin();
    const Route *lastRoute = NULL;

    for (; it != _routes.end(); it++)
    {
        if (startsWith(path, it->getRoute()) && (!lastRoute
            || it->getRoute().size() > lastRoute->getRoute().size())) {
            lastRoute = &(*it);
        }
    }

    if (!lastRoute) {
        throw ServerConfigException("No route was found");
    }

    return (lastRoute);
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

std::ostream &operator<<(std::ostream &os, const ServerConfig &sc)
{
    os << "------------------------------------------" << std::endl
       << std::endl;

    os << "   Name: " << (sc.getName().empty() ? "N/A" : sc.getName()) << std::endl;
    os << "   Port: " << sc.getPort() << std::endl;
    os << "   Host: " << sc.getHost() << std::endl;
    os << "   Root folder: '" << sc.getRoot() << "'" << std::endl;
    os << "   Index file: '" << sc.getIndex() << "'" << std::endl;
    os << "   Maximum request body size: " << sc.getMaxBodySize() << std::endl;
    os << "   Accepted HTTP methods: [";

    const std::vector<HttpMethod> methods = sc.getHTTPMethods();
    for (std::vector<HttpMethod>::const_iterator it = methods.begin(); it != methods.end(); it++)
        std::cout << (it == methods.begin() ? "" : ", ") << (HttpMethod (*it)).getKey();
    os << "]" << std::endl;

    os << "\n   Routes: " << std::endl;
    const std::vector<Route> routes = sc.getRoutes();
    for (std::vector<Route>::const_iterator it = routes.begin(); it != routes.end(); it++)
        std::cout << "\n"
                  << (*it) << std::endl;

    return (os);
}