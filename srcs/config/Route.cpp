#include "Route.hpp"

Route::Route(int maxBodySize, const std::string &route, const std::string &root, const std::string &index, const std::vector<HttpMethod> &methods, bool autoindex, bool alias)
{
    this->_cgi = false;
    this->_selected_cgi = 0;
    this->_max_body_size = maxBodySize;
    this->_autoindex = autoindex;
    this->_alias = alias;
    this->_route = route;
    this->_root = root;
    this->_index = index;
    this->_accepted_http_methods = methods;
}

void Route::update(const std::vector<Pair> &pairs)
{
    std::string tmp;

    // ROUTE
    this->validate("route", pairs, this->_route);

    if (this->_route.at(this->_route.size() - 1) == '*')
        this->_route.erase(this->_route.size() - 1);

    if (this->_route.at(0) != '/')
        throw RouteException("Route must start with '" + highlight("/") + "': '" + highlight(this->_route) + "'");

    if (this->_route.at(this->_route.size() - 1) == '/')
        throw RouteException("Route can't end with '" + highlight("/") + "': '" + highlight(this->_route) + "'");

    // MAX BODY SIZE
    this->validate("max_body_size", pairs, tmp, false);
    if (!tmp.empty())
    {
        this->stringToInt(tmp, this->_max_body_size, "max_body_size");
        if (this->_max_body_size < 0)
            throw RouteException("Max body size must be a positive number");
    }

    // REDIRECTION
    this->validate("redirection", pairs, this->_redirection, false);
    if (!this->_redirection.empty())
    {
        // ignore every others params
        return;
    }

    // ROOT
    this->validate("root", pairs, tmp, false);
    if (!tmp.empty())
    {
        this->_root = tmp;
        if (!isValidDirectory(this->_root))
            throw RouteException("Path in '" + highlight("root") + "' is not a valid directory path: '" + highlight(this->_root) + "'");
    }

    // ALIAS
    this->validate("alias", pairs, tmp, false);
    if (!tmp.empty())
    {
        if (tmp == "true")
            this->_alias = true;
        else if (tmp == "false")
            this->_alias = false;
        else
            throw RouteException("Invalid value in '" + highlight("allias") + "', expected a boolean value: '" + highlight(tmp) + "'");
    }

    // INDEX
    this->validate("index", pairs, tmp, false);
    if (!tmp.empty())
    {
        this->_index = tmp;

        std::ifstream file;
        std::string path = this->_root + (this->_root.at(this->_root.size() - 1) == '/' ? "" : "/") + this->_index;

        file.open(path.c_str());

        if (file.fail())
            throw RouteException("Can't open index file at '" + highlight(path) + "': '" + highlight(std::string(strerror(errno))) + "'");

        file.close();
    }

    // CGI
    this->validate("cgi-ext", pairs, tmp, false);
    if (!tmp.empty())
    {
        this->_cgi = true;

        try
        {
            const std::vector<std::string> ext = JSON::getValuesFromArray(tmp);

            for (std::vector<std::string>::const_iterator it = ext.begin(); it != ext.end(); it++)
            {
                if (!startsWith(*it, "*."))
                    throw RouteException("CGI extension in '" + highlight("cgi-ext") + "' is invalid, it should start with '" + highlight("*.") + "': '" + highlight(*it) + "'");

                if ((*it).size() <= 2)
                    throw RouteException("Expected file extension in '" + highlight("cgi-ext") + "': " + highlight("'" + (*it) + "'"));

                // remove '*' and push to vector
                this->_cgi_ext.push_back((*it).substr(1));
            }

            this->validate("cgi-exec", pairs, tmp);

            const std::vector<std::string> exec = JSON::getValuesFromArray(tmp);

            for (std::vector<std::string>::const_iterator it = exec.begin(); it != exec.end(); it++)
            {
                std::ifstream file;
                file.open((*it).c_str());

                if (file.fail())
                    throw RouteException("Can't open CGI executable file at '" + highlight(*it) + "': '" + highlight(std::string(strerror(errno))) + "'");

                file.close();

                this->_cgi_exec.push_back(*it);
            }
        }
        catch (const std::exception &e)
        {
            throw;
        }
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
            throw RouteException("Expected boolean value in '" + highlight("autoindex") + "': '" + tmp + "'");
    }

    try
    {
        // HTTP METHODS
        this->validate("allowed_methods", pairs, tmp, false);
        if (!tmp.empty())
        {
            this->_accepted_http_methods.clear();

            std::vector<std::string> methods = JSON::getValuesFromArray(tmp);
            if (!methods.size())
                throw RouteException("Expected at least one HTTP method in '" + highlight("allowed_methods") + "'");

            for (std::vector<std::string>::const_iterator it = methods.begin(); it != methods.end(); it++)
            {
                HttpMethod hm;

                try
                {
                    hm = HttpMethod::get(*it);
                }
                catch (const HttpMethod::EnumException &e)
                {
                    throw RouteException("Invalid HTTP Method in '" + highlight("allowed_methods") + "': '" + highlight(*it) + "'");
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

const std::string &Route::getRoute(void) const
{
    return (this->_route);
}

const std::string &Route::getRedirection(void) const
{
    return (this->_redirection);
}

const std::vector<std::string> &Route::getExtensions(void) const
{
    return (this->_cgi_ext);
}

const std::vector<std::string> &Route::getExecutables(void) const
{
    return (this->_cgi_exec);
}

bool Route::isCGI(void) const
{
    return (this->_cgi);
}

void Route::stringToInt(const std::string &str, int &result, const std::string &context)
{
    std::stringstream ss(str);
    ss >> result;

    if (ss.fail() || !ss.eof())
        throw RouteException("Value is not a valid int '" + highlight(str) + "' in '" + highlight(context) + "'");
}

void Route::validate(const std::string &key, const std::vector<Pair> &pairs, std::string &result, bool mandatory)
{
    bool exist = Pair::exist(key, pairs);

    if (mandatory && !exist)
        throw RouteException("No '" + highlight(key) + "' defined in route");
    else if (exist)
    {
        result = Pair::get(key, pairs).getValue();
        if (result.empty())
            throw RouteException("Expected value in '" + highlight(key) + "'");
    }
    else
        result = "";
}

Route::RouteException::RouteException(const std::string &message) : _message(std::string(RED) + "Route Error" + std::string(YELLOW) + " - " + message)
{
}

const char *Route::RouteException::what() const throw()
{
    return (_message.c_str());
}

std::ostream &operator<<(std::ostream &os, const Route &r)
{
    os << "      [\"" << highlight(r.getRoute(), false) << "\"]" << std::endl;
    if (!r.getRedirection().empty())
        os << "      Redirection: " << highlight(r.getRedirection(), false) << std::endl;
    else
    {
        os << "      Root folder: " << highlight(r.getRoot(), false) << std::endl;

        if (r.autoIndex())
            os << "      Auto index: " << highlight("true", false) << std::endl;

        if (r.isAlias())
            os << "      Alias: " << highlight("true", false) << std::endl;

        if (r.isCGI())
        {
            os << "      CGI extensions: ";
            const std::vector<std::string> ext = r.getExtensions();
            for (std::vector<std::string>::const_iterator it = ext.begin(); it != ext.end(); it++)
                std::cout << (it == ext.begin() ? "" : ", ") << highlight(*it, false);
            os << std::endl;

            os << "      CGI executables: ";
            const std::vector<std::string> exec = r.getExecutables();
            for (std::vector<std::string>::const_iterator it = exec.begin(); it != exec.end(); it++)
                std::cout << (it == exec.begin() ? "" : ", ") << highlight(*it, false);
            os << std::endl;
        }
        else
            os << "      Index file: " << highlight(r.getIndex(), false) << std::endl;
    }

    std::ostringstream tmp;

    tmp << r.getMaxBodySize();

    os << "      Max Request Body Size: " << highlight(tmp.str(), false) << std::endl;
    os << "      Accepted HTTP methods: ";

    const std::vector<HttpMethod> methods = r.getHTTPMethods();
    for (std::vector<HttpMethod>::const_iterator it = methods.begin(); it != methods.end(); it++)
        std::cout << (it == methods.begin() ? "" : ", ") << highlight(((HttpMethod)*it).getKey(), false);
    os << std::endl;

    return (os);
}