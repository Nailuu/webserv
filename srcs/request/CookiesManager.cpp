#include "CookiesManager.hpp"
#include "global.hpp"

#include <sstream>
#include <iostream>

CookiesManager::CookiesManager() {}

const std::string CookiesManager::generateCookiePage(const std::string &id) const
{
    std::string sanitized_id = id;
    sanitized_id = sanitized_id.substr(0, 16);

    std::ostringstream tmp;

    std::map<std::string, std::string>::const_iterator it = this->_cookies.find(sanitized_id);

    tmp << "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Set Username</title><script>function sendUsername(){let username=document.getElementById(\"usernameInput\").value;if(username===\"\"){document.getElementById(\"status\").textContent=\"Please enter a username.\";return;}fetch('/set-cookie',{method:'POST',headers:{'Content-Type':'text/plain',},body:username}).catch(error=>{document.getElementById(\"status\").textContent=error.message;});}</script></head><body><h1>Set Username</h1><h2>";
    tmp << "Current username: " << (it == this->_cookies.end() ? "N/A" : (*it).second) << "</h2>";
    tmp << "<label for=\"usernameInput\">Enter your username:</label><input type=\"text\" id=\"usernameInput\" placeholder=\"Enter username\"><button onclick=\"sendUsername()\">Send Username</button><p id=\"status\"></p></body></html>";

    return (tmp.str());
}

const std::string &CookiesManager::newCookie(const std::string &data)
{
    std::string id = generateRandomId();

    this->_cookies[id] = data;

    std::map<std::string, std::string>::const_iterator it = this->_cookies.find(id);

    return ((*it).first);
}