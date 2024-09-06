#include "CGI.hpp"

CGI::CGI(const std::string &root, const std::vector<HttpMethod> &methods)
{
    this->_root = root;
    this->_accepted_http_methods = methods;
}

void CGI::update(const std::vector<Pair> &pairs)
{
    std::string tmp;

    // TYPE
    this->validate("type", pairs, this->_type);
    if (!startsWith(this->_type, "*."))
        throw CGIException("Type in '" + highlight("type") + "' is invalid, it should start with '" + highlight("*.") + "': '" + highlight(this->_type) + "'");

    if (this->_type.size() <= 2)
        throw CGIException("Expected file extension: " + highlight("'" + this->_type + "'"));

    // remove '*'
    this->_type = this->_type.substr(1);

    // Check if this file extension is allowed in CGI
    try
    {
        AcceptedCGI::getByValue(this->_type);
    }
    catch (std::exception &e)
    {
        throw CGIException("CGI '" + highlight("type") + "' not allowed: '" + highlight(this->_type) + "'");
    }

    // ROOT
    this->validate("root", pairs, tmp, false);
    if (!tmp.empty())
    {
        this->_root = tmp;
        if (!isValidDirectory(this->_root))
            throw CGIException("Path in '" + highlight("root") + "' is not a valid directory path: '" + highlight(this->_root) + "'");
    }

    // EXEC
    this->validate("exec", pairs, this->_exec);

    std::ifstream file;

    // Check that the cgi executable is a valid file
    file.open(this->_exec.c_str());

    if (file.fail())
        throw CGIException("Checks failed when trying to open CGI executable at '" + highlight(this->_exec) + "': '" + highlight(std::string(strerror(errno))) + "'");

    file.close();

    try
    {
        // HTTP METHODS
        this->validate("allowed_methods", pairs, tmp, false);
        if (!tmp.empty())
        {
            this->_accepted_http_methods.clear();
            
            std::vector<std::string> methods = JSON::getValuesFromArray(tmp);
            if (!methods.size())
                throw CGIException("Expected at least one HTTP method in '" + highlight("allowed_methods") + "'");

            for (std::vector<std::string>::const_iterator it = methods.begin(); it != methods.end(); it++)
            {
                HttpMethod hm;

                try
                {
                    hm = HttpMethod::get(*it);
                }
                catch (const HttpMethod::EnumException &e)
                {
                    throw CGIException("Invalid HTTP Method in '" + highlight("allowed_methods") + "': '" + highlight(*it) + "'");
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

const std::string &CGI::getType(void) const
{
    return (this->_type);
}

const std::string &CGI::getExec(void) const
{
    return (this->_exec);
}

void CGI::validate(const std::string &key, const std::vector<Pair> &pairs, std::string &result, bool mandatory)
{
    bool exist = Pair::exist(key, pairs);

    if (mandatory && !exist)
        throw CGIException("No '" + highlight(key) + "' defined in CGI");
    else if (exist)
    {
        result = Pair::get(key, pairs).getValue();
        if (result.empty())
            throw CGIException("Expected value in '" + highlight(key) + "'");
    }
    else
        result = "";
}

CGI::CGIException::CGIException(const std::string &message) : _message(std::string(RED) + "CGI Error" + std::string(YELLOW) + " - " + message)
{
}

const char *CGI::CGIException::what() const throw()
{
    return (_message.c_str());
}

std::ostream &operator<<(std::ostream &os, const CGI &c)
{
    os << "      [\"" << highlight(c.getType(), false) << "\"]" << std::endl;
    os << "      Root folder: " << highlight(c.getRoot(), false) << std::endl;
    os << "      Executable: " << highlight(c.getExec(), false) << std::endl;

    os << "      Accepted HTTP methods: ";

    const std::vector<HttpMethod> methods = c.getHTTPMethods();
    for (std::vector<HttpMethod>::const_iterator it = methods.begin(); it != methods.end(); it++)
        std::cout << (it == methods.begin() ? "" : ", ") << highlight(((HttpMethod)*it).getKey(), false);
    os << std::endl;

    return (os);
}