#pragma once

#include <sstream>
#include <exception>

#include "ServerConfig.hpp"

#define MAX_HEADER 4096
#define READ_CHUNK 4096

class StreamReader
{
    private:
        const int _fd;
		char _buffer[READ_CHUNK + 1];
		std::ostringstream _header;
		size_t _headerPos;
		bool _headerReceived;
		std::ostringstream _body;
        size_t _bodySize;
    public:
        StreamReader();
        StreamReader(const int fd);
        ~StreamReader();
        StreamReader(const StreamReader &other);
        StreamReader operator=(const StreamReader &other);
		bool isHeaderReceived(void) const;
		size_t getBodySize(void) const;
		std::string getHeader(void) const;
		std::string getBody(void) const;
		void onReceive(const ServerConfig &config);
        class ReaderException : public std::exception
        {
        public:
            ReaderException(const std::string& message);
            virtual ~ReaderException() throw() {};
            virtual const char* what() const throw();
        private:
            const std::string _message;
        };
};
