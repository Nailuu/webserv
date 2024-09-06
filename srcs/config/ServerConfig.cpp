#include "ServerConfig.hpp"
#include "HttpStatusCode.hpp"

ServerConfig::ServerConfig()
{
    this->_max_body_size = DEFAULT_MAX_BODY_SIZE;
}

void ServerConfig::build(const std::vector<Pair> &pairs)
{
    std::string tmp;

    // PORT
    this->validate("port", pairs, tmp);
    this->stringToInt(tmp, this->_port, "port");
    if (this->_port < 0 || this->_port > 65535)
        throw ServerConfigException("Port is out of range (" + highlight("0-65535") + ")");

    // MAX BODY SIZE
    this->validate("max_body_size", pairs, tmp, false);
    if (!tmp.empty())
    {
        this->stringToInt(tmp, this->_max_body_size, "max_body_size");
        if (this->_max_body_size < 0)
            throw ServerConfigException("Max body size must be a positive number");
    }

    // HOST
    this->validate("host", pairs, this->_host);

    // NAME
    this->validate("server_name", pairs, this->_name, false);

    // ROOT
    this->validate("root", pairs, this->_root);
    if (!isValidDirectory(this->_root))
        throw ServerConfigException("Path in '" + highlight("root") + "' is not a valid directory path: '" + highlight(this->_root) + "'");

    // ALIAS
    this->validate("alias", pairs, tmp, false);
    if (!tmp.empty())
    {
        if (tmp == "true")
            this->_alias = true;
        else if (tmp == "false")
            this->_alias = false;
        else
            throw ServerConfigException("Invalid value in '" + highlight("allias") + "', expected a boolean value: '" + highlight(tmp) + "'");
    }

    // INDEX
    this->validate("index", pairs, this->_index, false);
    if (!this->_index.empty())
    {
        std::ifstream file;
        std::string path = this->_root + "/" + this->_index;

        file.open(path.c_str());

        if (file.fail())
            throw ServerConfigException("Can't open index file at '" + highlight(path) + "': '" + highlight(std::string(strerror(errno))) + "'");

        file.close();
    }

    // DIRECTORY LISTING "autoindex"
    this->validate("autoindex", pairs, tmp, false);
    if (!tmp.empty())
    {
        if (tmp == "true")
            this->_autoindex = true;
        else if (tmp == "false")
            this->_autoindex = false;
        else
            throw ServerConfigException("Expected boolean value in '" + highlight("autoindex") + "': '" + highlight(tmp) + "'");
    }

    try
    {
        // HTTP METHODS
        this->validate("allowed_methods", pairs, tmp);

        std::vector<std::string> methods = JSON::getValuesFromArray(tmp);
        if (!methods.size())
            throw ServerConfigException("Expected at least one HTTP method in '" + highlight("allowed_methods") + "'");

        for (std::vector<std::string>::const_iterator it = methods.begin(); it != methods.end(); it++)
        {
            HttpMethod hm;

            try
            {
                hm = HttpMethod::get(*it);
            }
            catch (const HttpMethod::EnumException &e)
            {
                throw ServerConfigException("Invalid HTTP Method in '" + highlight("allowed_methods") + "': '" + highlight(*it) + "'");
            }

            this->_accepted_http_methods.push_back(hm);
        }

        // ROUTES
        // create default route on "/" based on default settings
        this->_routes.push_back(Route(this->_max_body_size, "/", this->_root, this->_index, this->_accepted_http_methods, this->_autoindex, this->_alias));

        // Create more routes if 'routes' is defined in the configuration JSON
        this->validate("routes", pairs, tmp, false);
        if (!tmp.empty())
        {
            std::vector<std::string> routes = JSON::getObjectsFromArray(tmp);
            for (std::vector<std::string>::const_iterator it = routes.begin(); it != routes.end(); it++)
            {
                // Initialize route with default settings
                Route r(this->_max_body_size, "unknown", this->_root, this->_index, this->_accepted_http_methods, this->_autoindex, this->_alias);

                // Overwrite default settings
                std::vector<Pair> p = JSON::getKeysAndValuesFromObject(*it);
                r.update(p);

                if (this->getRoute(r.getRoute(), true))
                    throw ServerConfigException("Duplicate route in '" + highlight("routes") + "': '" + highlight(r.getRoute()) + "'");

                this->_routes.push_back(r);
            }
        }

        // ERRORS Default Pages
        this->validate("errors", pairs, tmp, false);
        if (!tmp.empty())
        {
            int x;

            std::vector<std::string> tmp2 = JSON::getObjectsFromArray(tmp);
            for (std::vector<std::string>::const_iterator it = tmp2.begin(); it != tmp2.end(); it++)
            {
                std::vector<Pair> pairs2 = JSON::getKeysAndValuesFromObject(*it);

                std::string code;
                this->validate("code", pairs2, code);

                // Check if it's a valid int
                this->stringToInt(code, x, "errors");

                HttpStatusCode *ptr = NULL;

                try
                {
                    // Check that the code is a valid http code
                    HttpStatusCode &status = HttpStatusCode::getByValue(x);

                    ptr = &status;
                }
                catch (const std::exception &e)
                {
                    throw ServerConfigException("HTTP Error code is not valid in '" + highlight("errors") + "': '" + highlight(code) + "'");
                }

                this->validate("file", pairs2, tmp);

                // Check that the path in key is a valid file
                std::ifstream file;

                file.open(tmp.c_str());

                if (file.fail())
                    throw ServerConfigException("Can't open file at '" + highlight(tmp) + "' for error '" + highlight(code) + "': '" + highlight(std::string(strerror(errno))) + "'");

                ptr->setPath(tmp);

                file.close();
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

const std::string &ServerConfig::getHost(void) const
{
    return (this->_host);
}

const std::string &ServerConfig::getName(void) const
{
    return (this->_name);
}

const std::vector<Route> &ServerConfig::getRoutes(void) const
{
    return (this->_routes);
}

const Route *ServerConfig::getRoute(const std::string &path, bool duplicate) const
{
    if (_routes.empty())
    {
        throw ServerConfigException("No route was registered");
    }

    std::vector<Route>::const_iterator it = _routes.begin();
    const Route *lastRoute = NULL;

    for (; it != _routes.end(); it++)
    {
        if (duplicate && it->getRoute() == path)
            return (&(*it));

        else if (!duplicate)
        {
            std::string route = (it->getRoute().at(it->getRoute().size() - 1) == '/' || it->getRoute() == path)
                                    ? it->getRoute()
                                    : it->getRoute() + "/";
            if (startsWith(path, route) && (!lastRoute || it->getRoute().size() > lastRoute->getRoute().size()))
            {
                lastRoute = &(*it);
            }
        }
    }

    if (!duplicate && !lastRoute)
    {
        throw ServerConfigException("No route was found");
    }

    return (lastRoute);
}

void ServerConfig::validate(const std::string &key, const std::vector<Pair> &pairs, std::string &result, bool mandatory)
{
    bool exist = Pair::exist(key, pairs);

    if (mandatory && !exist)
        throw ServerConfigException("No '" + highlight(key) + "' defined in server configuration");
    else if (exist)
    {
        result = Pair::get(key, pairs).getValue();
        if (result.empty())
            throw ServerConfigException("Expected value in '" + highlight(key) + "'");
    }
    else
        result = "";
}

void ServerConfig::stringToInt(const std::string &str, int &result, const std::string &context)
{
    std::stringstream ss(str);
    ss >> result;

    if (ss.fail() || !ss.eof())
        throw ServerConfigException("Value is not a valid int '" + highlight(str) + "' in '" + highlight(context) + "'");
}

ServerConfig::ServerConfigException::ServerConfigException(const std::string &message) : _message(std::string(RED) + "Server Config Error" + std::string(YELLOW) + " - " + message)
{
}

const char *ServerConfig::ServerConfigException::what() const throw()
{
    return (_message.c_str());
}

std::ostream &operator<<(std::ostream &os, const ServerConfig &sc)
{
    os << GREY << "------------------------------------------" << WHITE << std::endl
       << std::endl;

    os << "   Name: " << highlight(sc.getName().empty() ? "N/A" : sc.getName(), false) << std::endl;

    std::ostringstream tmp;
    tmp << sc.getPort();
    os << "   Port: " << highlight(tmp.str(), false) << std::endl;

    os << "   Host: " << highlight(sc.getHost(), false) << std::endl;
    os << "   Root folder: " << highlight(sc.getRoot(), false) << std::endl;
    os << "   Index file: " << highlight(sc.getIndex(), false) << std::endl;
    os << "   Auto index: " << highlight(sc.autoIndex() ? "true" : "false", false) << std::endl;
    os << "   Alias: " << highlight(sc.isAlias() ? "true" : "false", false) << std::endl;

    tmp.str("");
    tmp.clear();
    tmp << sc.getMaxBodySize();
    os << "   Maximum request body size: " << highlight(tmp.str(), false) << std::endl;
    os << "   Accepted HTTP methods: ";

    const std::vector<HttpMethod> methods = sc.getHTTPMethods();
    for (std::vector<HttpMethod>::const_iterator it = methods.begin(); it != methods.end(); it++)
        std::cout << (it == methods.begin() ? "" : ", ") << highlight((HttpMethod(*it)).getKey(), false);
    os << std::endl;

    os << YELLOW << "\n   Routes: " << WHITE << std::endl;
    const std::vector<Route> routes = sc.getRoutes();
    for (std::vector<Route>::const_iterator it = routes.begin(); it != routes.end(); it++)
        std::cout << "\n"
                  << (*it) << std::endl;

    return (os);
}