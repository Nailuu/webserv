#include "Route.hpp"

Route::Route(int maxBodySize, const std::string &route, const std::string &root, const std::string &index, const std::vector<HTTP_METHOD> &methods)
{
    this->_max_body_size = maxBodySize;
    this->_route = route;
    this->_root = root;
    this->_index = index;
    this->_accepted_http_methods = methods;
}

void Route::update(const std::vector<Pair>& pairs)
{
    VectorDebugger<Pair>::print(pairs);
}

int Route::getMaxBodySize() const
{
    return (this->_max_body_size);
}

const std::string &Route::getRoute() const
{
    return (this->_route);
}

const std::string &Route::getRoot() const
{
    return (this->_root);
}

const std::string &Route::getIndex() const
{
    return (this->_index);
}

const std::vector<HTTP_METHOD> &Route::getHTTPMethods() const
{
    return (this->_accepted_http_methods);
}

Route::RouteException::RouteException(const std::string &message) : _message("Route Error - " + message)
{
}

const char *Route::RouteException::what() const throw()
{
    return (_message.c_str());
}