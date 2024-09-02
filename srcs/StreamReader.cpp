#include "StreamReader.hpp"
#include <cstdio>
#include <unistd.h>

StreamReader::StreamReader(const int fd) : _fd(fd), _headerPos(0), _headerReceived(false), _bodySize(0) {}

StreamReader::~StreamReader() {}

StreamReader::StreamReader(const StreamReader &other) : _fd(other._fd), _headerPos(other._headerPos), _headerReceived(other._headerReceived), _bodySize(other._bodySize)
{
	_header << other._header.str();
    _body << other._body.str();
}

StreamReader StreamReader::operator=(const StreamReader &other)
{
    if (&other != this)
    {
		_headerPos = other._headerPos;
		_headerReceived = other._headerReceived;
		_bodySize = other._bodySize;
		_header << other._header.str();
    	_body << other._body.str();
    }
    return (*this);
}

bool StreamReader::isHeaderReceived(void) const
{
	return (this->_headerReceived);
}

size_t StreamReader::getBodySize(void) const
{
	return (this->_bodySize);
}

std::string StreamReader::getHeader(void) const
{
	return (this->_header.str().substr(0, _headerPos));
}

std::string StreamReader::getBody(void) const
{
	return (this->_body.str());
}

void StreamReader::onReceive(const ServerConfig &config)
{
	size_t read_size = READ_CHUNK + (_headerReceived == false);
	
	ssize_t ambigous_readed = read(_fd, _buffer, read_size);

	if (ambigous_readed < 1)
		throw ReaderException("Input was cut");
	
	size_t readed = (size_t) ambigous_readed;
	_buffer[readed] = 0;
	
	if (!_headerReceived)
	{
		_header << _buffer;

		std::string header_str = _header.str();

		size_t pos = header_str.find("\r\n\r\n");
		size_t max_length = MAX_HEADER - 4;

		if ((pos == std::string::npos && header_str.length() > max_length)
		|| pos > max_length)
			throw ReaderException("Header is too big");
		
		if (pos != std::string::npos)
		{
			_headerReceived = true;
			_headerPos = pos + 4;

			_body << header_str.substr(_headerPos);
			_bodySize = _body.str().length();
		}

		return;
	}

	_body << _buffer;
	_bodySize = _body.str().length();

	if (_bodySize > (size_t) config.getMaxBodySize())
		throw ReaderException("Header is too big");
}

StreamReader::ReaderException::ReaderException(const std::string &message) : _message(std::string(RED) + "Client Error" + std::string(YELLOW) + " - " + message) {}

const char *StreamReader::ReaderException::what() const throw()
{
    return (_message.c_str());
}
