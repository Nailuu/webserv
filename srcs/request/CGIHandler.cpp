#include "ServerManager.hpp"
#include "global.hpp"

#include <cstdlib>
#include <ostream>
#include <map>

CGIHandler::CGIHandler() : _envs()
{
    this->_active = false;
    this->_timedout = false;
    this->_envp = 0;
    this->_args = 0;
}

void CGIHandler::init(const ServerConfig &config, const Request &req, const Route *route, const std::string &path, int CGIExecutableIndex)
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
    this->_envs["CGI_SOFTWARE"] = route->getExecutables().at(CGIExecutableIndex);

    std::ostringstream tmp2;
    tmp2 << config.getPort();

    this->_envs["SERVER_PORT"] = tmp2.str();
    this->_envs["GATEWAY_INTERFACE"] = "CGI/1.1";
}

void CGIHandler::execute(ServerManager *manager)
{
    // this->print();

    this->_active = true;

    int output_pipe[2];
    int error_pipe[2];
    int timeout_pipe[2];

    if (pipe(output_pipe) == -1 || pipe(error_pipe) == -1 || pipe(timeout_pipe) == -1)
        throw CGIHandlerException("Cannot create pipes");

    this->_pid = fork();

    if (this->_pid == -1)
        throw CGIHandlerException("Cannot create fork");

    try
    {
        // 1st fork
        if (this->_pid == 0)
        {
            int fork_pipe[2];

            if (pipe(fork_pipe) == -1)
                throw CGIHandlerException("Cannot create pipes");

            pid_t pid2 = fork();

            if (pid2 == -1)
                throw CGIHandlerException("Cannot create sub-fork");

            // 2nd fork
            else if (pid2 == 0)
            {
                // Redirect STDOUT and STDERR
                dup2(fork_pipe[1], STDOUT_FILENO);
                dup2(fork_pipe[1], STDERR_FILENO);

                close(fork_pipe[0]);

                this->init_envp();
                this->init_args();

                execve(this->_args[0], this->_args, this->_envp);

                // Free memory of envp and args if execve has failed
                this->clean();

                // If execve failed the execution will continue
                std::cout << strerror(errno);

                // Clear heap
                manager->clear();
                exit(1);
            }

            // 1st fork
            else
            {
                fd_set readfds;
                FD_ZERO(&readfds);
                FD_SET(fork_pipe[0], &readfds);

                fd_set writefds;
                FD_ZERO(&writefds);

                char buffer[16384] = {0};
                std::ostringstream tmp;

                struct timeval timeout;
                timeout.tv_sec = TIMEOUT;
                timeout.tv_usec = 0;

                bool timedout = false;

                // Wait for script output with a timeout
                int activity = select(fork_pipe[0] + 1, &readfds, NULL, NULL, &timeout);
                if (activity == 0)
                    timedout = true;
                
                // Assure all pipes are ready

                FD_ZERO(&writefds);
                FD_SET(timeout_pipe[1], &writefds);
                FD_SET(error_pipe[1], &writefds);
                FD_SET(output_pipe[1], &writefds);

                timeout.tv_sec = 0;
                timeout.tv_usec = 0;

                int max_fd = std::max(std::max(timeout_pipe[1], error_pipe[1]), output_pipe[1]);

                if (select(max_fd + 1, NULL, &writefds, NULL, &timeout) != 3)
                    throw CGIHandlerException("Write Pipes are not ready");

                if (!timedout && FD_ISSET(fork_pipe[0], &readfds))
                {
                    // While loop for reading blocking for some strange reasons????
                    int readed = read(fork_pipe[0], buffer, sizeof(buffer));

                    if (readed > 0)
                        tmp << buffer;
                    else if (readed == -1)
                        throw CGIHandlerException("Error reaading from fork output");
                }

                if (timedout)
                    kill(pid2, SIGKILL);

                // Wait for the child process to finish
                int status;
                waitpid(pid2, &status, 0);

                std::string output = tmp.str();

                // Timedout
                if (timedout)
                    write(timeout_pipe[1], "true", 5);

                // Error
                else if (!WIFSIGNALED(status) && WEXITSTATUS(status) != 0)
                    write(error_pipe[1], output.c_str(), output.size() + 1);

                // No Error
                else
                    write(output_pipe[1], output.c_str(), output.size() + 1);

                close(fork_pipe[1]);
                close(timeout_pipe[1]);
                close(error_pipe[1]);
                close(output_pipe[1]);
                close(fork_pipe[0]);
                close(timeout_pipe[0]);
                close(error_pipe[0]);
                close(output_pipe[0]);

                // Clear heap
                manager->clear();

                exit(0);
            }
        }
        else
        {
            close(output_pipe[1]);
            close(error_pipe[1]);
            close(timeout_pipe[1]);

            this->_output_pipe = output_pipe[0];
            this->_error_pipe = error_pipe[0];
            this->_timeout_pipe = timeout_pipe[0];
        }
    }
    catch (const std::exception &e)
    {
        throw;
    }
}

const std::string &CGIHandler::getByKey(const std::string &key) const
{
    return (this->_envs.at(key));
}

bool CGIHandler::isActive(void) const
{
    return (this->_active);
}

const pid_t &CGIHandler::getPid(void) const
{
    return (this->_pid);
}

bool CGIHandler::hasErrors(void) const
{
    return (!this->_errors.empty());
}

bool CGIHandler::hasTimedOut(void) const
{
    return (this->_timedout);
}

const std::string &CGIHandler::getOutput(void) const
{
    return (this->_output);
}

const std::string &CGIHandler::getErrors(void) const
{
    return (this->_errors);
}

void CGIHandler::readPipes(void)
{
    this->_active = false;

    char buffer[4096] = {0};

    // Ensure at least one FD is ready
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(this->_timeout_pipe, &readfds);
    FD_SET(this->_error_pipe, &readfds);
    FD_SET(this->_output_pipe, &readfds);

    int max_fd = std::max(std::max(this->_timeout_pipe, this->_error_pipe), this->_output_pipe);

    struct timeval timeout_val;
    timeout_val.tv_sec = 0;
    timeout_val.tv_usec = 0;

    if (select(max_fd + 1, &readfds, NULL, NULL, &timeout_val) == 0)
        throw CGIHandlerException("Read Pipes are not ready");

    std::ostringstream timeout;
    std::ostringstream errors;
    std::ostringstream output;

    ssize_t bytesRead = 0;

    if (FD_ISSET(this->_timeout_pipe, &readfds))
    {
        while ((bytesRead = read(this->_timeout_pipe, buffer, sizeof(buffer))) > 0)
            timeout << buffer;

        if (bytesRead == -1)
            throw CGIHandlerException("Read failed");

        if (!timeout.str().empty())
        {
            this->_timedout = true;

            close(this->_output_pipe);
            close(this->_error_pipe);
            close(this->_timeout_pipe);

            return;
        }
    }

    if (FD_ISSET(this->_error_pipe, &readfds))
    {
         while ((bytesRead = read(this->_error_pipe, buffer, sizeof(buffer))) > 0)
            errors << buffer;

        if (bytesRead == -1)
            throw CGIHandlerException("Read failed");

        if (!errors.str().empty())
        {
            this->_errors = errors.str();

            close(this->_output_pipe);
            close(this->_error_pipe);
            close(this->_timeout_pipe);

            return;
        }
    }

    if (FD_ISSET(this->_output_pipe, &readfds))
    {
        while ((bytesRead = read(this->_output_pipe, buffer, sizeof(buffer))) > 0)
            output << buffer;

        if (bytesRead == -1)
            throw CGIHandlerException("Read failed");

        if (!output.str().empty())
        {
            this->_output = output.str();

            close(this->_output_pipe);
            close(this->_error_pipe);
            close(this->_timeout_pipe);

            return;
        }
    }
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

    result[index] = NULL;
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