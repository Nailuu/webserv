#include "CGIENV.hpp"
#include "global.hpp"

#include <cstdlib>
#include <ostream>
#include <map>

CGIENV::CGIENV() : _envs(), _c_envs(NULL) {}

CGIENV::CGIENV(const ServerConfig &config, const Request &req, const CGI *cgi) : _envs(), _c_envs(NULL)
{
    // https://www.tutorialspoint.com/python/python_cgi_programming.htm

    this->_envs["CONTENT_TYPE"] = req.getFieldsValueByName("Content-Type");

    std::string tmp = req.getFieldsValueByName("Content-Length");
    if (tmp.empty())
        tmp = "0";
    this->_envs["CONTENT_LENGTH"] = tmp;
    this->_envs["HTTP_COOKIE"] = "";
    this->_envs["HTTP_USER_AGENT"] = req.getFieldsValueByName("User-Agent");
    this->_envs["PATH_INFO"] = req.getPath();
    this->_envs["QUERY_STRING"] = req.getParams();
    this->_envs["REMOTE_ADDR"] = req.getHost();
    this->_envs["REMOTE_HOST"] = req.getHost();
    this->_envs["REQUEST_METHOD"] = req.getMethod().getKey();
    this->_envs["SCRIPT_FILENAME"] = cgi->getExec();
    this->_envs["SCRIPT_NAME"] = req.getPath().substr(req.getPath().find('/') + 1);
    this->_envs["SERVER_NAME"] = config.getHost();
    this->_envs["SERVER_SOFTWARE"] = SERVER_NAME;
    this->_envs["SERVER_PROTOCOL"] = "HTTP/1.1";
    this->_envs["GATEWAY_INTERFACE"] = "CGI/1.1";
}

CGIENV::~CGIENV()
{
    if (this->_c_envs == NULL)
        return;

    for (int i = 0; this->_c_envs[i]; i++)
        delete[] (this->_c_envs[i]);

    delete[] (this->_c_envs);
}

CGIENV CGIENV::operator=(const CGIENV &other)
{
    if (this != &other)
    {
        this->_envs = other._envs;
    }

    return (*this);
}

void CGIENV::print(void) const
{
    std::cout << "CGI Environment variables: \n"
              << std::endl;

    for (std::map<std::string, std::string>::const_iterator it = this->_envs.begin(); it != this->_envs.end(); it++)
        std::cout << it->first << ": " << highlight(it->second, false) << std::endl;
}

char **CGIENV::getEnvs(void)
{
    char **result = new char *[this->_envs.size() + 1];
    result[this->_envs.size()] = NULL;

    int index = 0;
    for (std::map<std::string, std::string>::const_iterator it = this->_envs.begin(); it != this->_envs.end(); it++)
    {
        std::string key_value = it->first + "=" + it->second;

        result[index] = new char[key_value.size() + 1];
        strcpy(result[index], key_value.c_str());

        index++;
    }

    this->_c_envs = result;
    return (result);
}

const std::string &CGIENV::getByKey(const std::string &key) const
{
    return (this->_envs.at(key));
}

CGIENV::CGIENVException::CGIENVException(const std::string &message) : _message(std::string(RED) + "CGIENV Error" + std::string(YELLOW) + " - " + message) {}

const char *CGIENV::CGIENVException::what() const throw()
{
    return (_message.c_str());
}