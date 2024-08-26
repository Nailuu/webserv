#include "Route.hpp"

Route::Route(int maxBodySize, const std::string &route, const std::string &root, const std::string &index, const std::vector<HttpMethod> &methods)
{
    this->_max_body_size = maxBodySize;
    this->_route = route;
    this->_root = root;
    this->_index = index;
    this->_accepted_http_methods = methods;
}

void Route::update(const std::vector<Pair> &pairs)
{
    // VectorDebugger<Pair>::print(pairs);
    std::string tmp;

    // ROUTE
    this->validate("route", pairs, this->_route);
    if (this->_route.at(0) != '/')
        throw RouteException("Route must start with '/': '" + this->_route + "'");

    // MAX BODY SIZE
    this->validate("max_body_size", pairs, tmp, false);
    if (!tmp.empty())
    {
        this->stringToInt(tmp, this->_max_body_size, "max_body_size");
        if (this->_max_body_size < 0)
            throw RouteException("Max body size must be a positive number");
    }

    // ROOT
    this->validate("root", pairs, tmp, false);
    if (!tmp.empty())
    {
        this->_root = tmp;
        if (!isValidDirectory(this->_root))
            throw RouteException("Path in 'root' is not a valid directory path: '" + this->_root + "'");
    }

    // INDEX
    this->validate("index", pairs, tmp, false);
    if (!tmp.empty())
    {
        this->_index = tmp;

        std::ifstream file;
        std::string path = this->_root + "/" + this->_index;

        file.open(path.c_str());

        if (file.fail())
            throw RouteException("Can't open index file at '" + path + "': " + std::string(strerror(errno)));
    }

    try
    {
        // HTTP METHODS
        this->validate("allowed_methods", pairs, tmp, false);
        if (!tmp.empty())
        {
            std::vector<std::string> methods = JSON::getValuesFromArray(tmp);
            if (!methods.size())
                throw RouteException("Expected at least one HTTP method in 'allowed_methods'");

            for (std::vector<std::string>::const_iterator it = methods.begin(); it != methods.end(); it++)
            {
                HttpMethod hm;

                try {
                    hm = HttpMethod::get(*it);
                } catch (const HttpMethod::EnumException &e) {
                    throw RouteException("Invalid HTTP Method in 'allowed_methods': '" + *it + "'");
                }

                this->_accepted_http_methods.push_back(hm);
            }
        }
    }
    catch (std::exception &e)
    {
        throw;
    }
}

int Route::getMaxBodySize(void) const
{
    return (this->_max_body_size);
}

const std::string &Route::getRoute(void) const
{
    return (this->_route);
}

const std::string &Route::getRoot(void) const
{
    return (this->_root);
}

const std::string &Route::getIndex(void) const
{
    return (this->_index);
}

const std::vector<HttpMethod> &Route::getHTTPMethods(void) const
{
    return (this->_accepted_http_methods);
}

bool Route::isHTTPMethodAuthorized(HttpMethod method) const
{
    std::vector<HttpMethod>::const_iterator it = this->_accepted_http_methods.begin();
    for (; it != this->_accepted_http_methods.end(); it++)
    {
        if ((*it) == method)
            return (true);
    }

    return (false);
}

void Route::stringToInt(const std::string &str, int &result, const std::string &context)
{
    std::stringstream ss(str);
    ss >> result;

    if (ss.fail() || !ss.eof())
        throw RouteException("Value is not a valid int '" + str + "' in '" + context + "'");
}

void Route::validate(const std::string &key, const std::vector<Pair> &pairs, std::string &result, bool mandatory)
{
    bool exist = Pair::exist(key, pairs);

    if (mandatory && !exist)
        throw RouteException("No '" + key + "' defined in route");
    else if (exist)
    {
        result = Pair::get(key, pairs).getValue();
        if (result.empty())
            throw RouteException("Expected value in '" + key + "'");
    }
    else
        result = "";
}

Route::RouteException::RouteException(const std::string &message) : _message("Route Error - " + message)
{
}

const char *Route::RouteException::what() const throw()
{
    return (_message.c_str());
}

std::ostream &operator<<(std::ostream &os, const Route &r)
{
    os << "      [\"" << r.getRoute() << "\"]" << std::endl;
    os << "      Root folder: '" << r.getRoot() << "'" << std::endl;
    os << "      Index file: '" << r.getIndex() << "'" << std::endl;
    os << "      Max Request Body Size: " << r.getMaxBodySize() << std::endl;
    os << "      Accepted HTTP methods: [";

    const std::vector<HttpMethod> methods = r.getHTTPMethods();
    for (std::vector<HttpMethod>::const_iterator it = methods.begin(); it != methods.end(); it++)
        std::cout << (it == methods.begin() ? "" : ", ") << ((HttpMethod) *it).getKey();
    os << "]" << std::endl;

    return (os);
}