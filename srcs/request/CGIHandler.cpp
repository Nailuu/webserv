#include "CGIHandler.hpp"
#include "global.hpp"

#include <cstdlib>
#include <ostream>
#include <map>
#include <sys/wait.h>

CGIHandler::CGIHandler() : _envs(), _envp(NULL), _args(NULL) {}

void CGIHandler::init(const ServerConfig &config, const Request &req, const Route *route, const std::string &path)
{
    // https://www.tutorialspoint.com/python/python_cgi_programming.htm

    this->_envs["CONTENT_TYPE"] = req.getFieldsValueByName("Content-Type");

    std::string tmp = req.getFieldsValueByName("Content-Length");
    if (tmp.empty())
        tmp = "0";
    this->_envs["CONTENT_LENGTH"] = tmp;

    this->_envs["HTTP_COOKIE"] = "";
    this->_envs["HTTP_ACCEPT"] = req.getFieldsValueByName("Accept");
    this->_envs["HTTP_USER_AGENT"] = req.getFieldsValueByName("User-Agent");
    this->_envs["PATH_INFO"] = req.getPath();
    this->_envs["QUERY_STRING"] = req.getParams();
    this->_envs["REMOTE_ADDR"] = req.getHost();
    this->_envs["REQUEST_METHOD"] = req.getMethod().getKey();
    this->_envs["SCRIPT_FILENAME"] = path;
    this->_envs["SCRIPT_NAME"] = req.getPath().substr(req.getPath().find('/') + 1);
    this->_envs["SERVER_NAME"] = config.getHost();
    this->_envs["SERVER_SOFTWARE"] = SERVER_NAME;
    this->_envs["SERVER_PROTOCOL"] = "HTTP/1.1";
    this->_envs["CGI_SOFTWARE"] = route->getExecutable();

    std::ostringstream tmp2;
    tmp2 << config.getPort();

    this->_envs["SERVER_PORT"] = tmp2.str();
    this->_envs["GATEWAY_INTERFACE"] = "CGI/1.1";
}

const std::string CGIHandler::execute(bool &timedout)
{
    // this->print();

    int pipeIn[2];  // Input
    int pipeOut[2]; // Output

    if (pipe(pipeIn) == -1 || pipe(pipeOut) == -1)
        throw CGIHandlerException("Cannot create pipes");

    pid_t pid = fork();

    if (pid == -1)
        throw CGIHandlerException("Cannot create fork");

    else if (pid == 0)
    {
        // Redirect STDIN and STDOUT
        dup2(pipeIn[0], STDIN_FILENO);
        dup2(pipeOut[1], STDOUT_FILENO);
        dup2(pipeOut[1], STDERR_FILENO);

        close(pipeIn[1]);
        close(pipeOut[0]);

        this->init_envp();
        this->init_args();

        execve(this->_args[0], this->_args, this->_envp);

        // If execve failed the execution will continue
        std::cout << strerror(errno);
        exit(1);
    }
    else
    {
        close(pipeIn[0]);
        close(pipeOut[1]);
        close(pipeIn[1]);

        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(pipeOut[0], &readfds);

        char buffer[4096] = {0};
        std::ostringstream output;

        struct timeval timeout;
        timeout.tv_sec = TIMEOUT;
        timeout.tv_usec = 0;

        // Wait for script output with a timeout
        int activity = select(pipeOut[0] + 1, &readfds, NULL, NULL, &timeout);
        if (activity < 0)
            throw CGIHandlerException("Error during select: " + std::string(strerror(errno)));
        else if (activity == 0)
            timedout = true;

        if (!timedout && FD_ISSET(pipeOut[0], &readfds))
        {
            ssize_t bytesRead = read(pipeOut[0], buffer, sizeof(buffer) - 1);
            if (bytesRead > 0)
            {
                buffer[bytesRead] = '\0';
                output << buffer;
            }
            else if (bytesRead < 0)
                throw CGIHandlerException("Error reading from pipe: '" + highlight(std::string(strerror(errno))) + "'");
        }

        if (timedout)
            kill(pid, SIGKILL);

        close(pipeOut[0]);

        // Wait for the child process to finish
        int status;
        waitpid(pid, &status, 0);

        // Free memory of envp and args
        this->clean();

        if (!WIFSIGNALED(status) && WEXITSTATUS(status) != 0)
            throw CGIHandlerException("---\n" + output.str());

        return (output.str());
    }
}

const std::string &CGIHandler::getByKey(const std::string &key) const
{
    return (this->_envs.at(key));
}

void CGIHandler::init_envp(void)
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

    this->_envp = result;
}

void CGIHandler::init_args(void)
{
    std::string tmp = this->_envs["CGI_SOFTWARE"];

    char *arg1 = new char[tmp.size() + 1];
    strcpy(arg1, tmp.c_str());

    tmp = this->_envs["SCRIPT_FILENAME"];
    char *arg2 = new char[tmp.size() + 1];
    strcpy(arg2, tmp.c_str());

    char **result = new char *[3];
    result[0] = arg1;
    result[1] = arg2;
    result[2] = NULL;

    this->_args = result;
}

void CGIHandler::clean(void)
{
    if (this->_envp != NULL)
    {
        for (int i = 0; this->_envp[i]; i++)
            delete[] (this->_envp[i]);

        delete[] (this->_envp);
    }

    if (this->_args != NULL)
    {
        for (int i = 0; this->_args[i]; i++)
            delete[] (this->_args[i]);

        delete[] (this->_args);
    }
}

CGIHandler::CGIHandlerException::CGIHandlerException(const std::string &message) : _message(message) {}

const char *CGIHandler::CGIHandlerException::what() const throw()
{
    return (_message.c_str());
}