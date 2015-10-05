#include "socket.h"

Socket::Socket()
{
	this->handle = socket(AF_INET, SOCK_STREAM, 0);
	this->line_index = 0;
    this->connected = false;
	this->line_buf[0] = 0;

	#ifdef SOCKET_CHECK_ERRORS
	if (this->handle < 0)
	{
		HelpPrinter::print(LOG_ERROR, "error creating socket %d\n", errno);
	}
	#endif
}

Socket::Socket(int handle) : handle(handle)
{
	this->line_index = 0;
    this->connected = true;
	this->line_buf[0] = 0;
}

Socket::~Socket()
{
	this->close();
}

void Socket::disconnect()
{
    this->connected = false;
}

int Socket::get_handle()
{
	return this->handle;
}

void Socket::bind(unsigned short port)
{
	int opt = 1;
	int result = setsockopt(this->handle, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	
	if (result != 0)
	{
		printf("error setting socket options, error %d\n", errno);
	}
	
	const sockaddr_in address = Socket::create_address(port);
	result = ::bind(this->handle, (const sockaddr*) &address, sizeof(address));
	
	if (result != 0)
	{
		printf("error binding socket %d\n", errno);
	}
}

bool Socket::connect(const char* host, unsigned short port)
{
	hostent *hostip = gethostbyname(host);
	
	if (!hostip)
	{
		printf("couldn't get host from %hu, error %d\n", port, errno);
	}

	sockaddr_in address = Socket::create_address(*(in_addr*) hostip->h_addr_list[0], port);
	
	int result = ::connect(this->handle, (const sockaddr*) &address, sizeof(address));
	
	if (result != 0)
	{
		printf("couldn't connect to %s %hu, error %d\n", host, port, errno);
	}
    
    if (result == 0)
    {
		this->connected = true;
	}
	
	return !result;
}

bool Socket::write(const char* string)
{
	return this->write(string, strlen(string) + 1);
}
bool Socket::write(const char* buffer, size_t length)
{
	int result = ::write(this->handle, buffer, length);
	
	#ifdef SOCKET_CHECK_ERRORS
	if (result < 0)
	{
		HelpPrinter::print(LOG_ERROR, "couldn't send data through socket %d, error %d\n", this->handle, errno);
	}
	#endif
	
    if (result < 1 && length > 0)
    {
        this->disconnect();
    }
    
	return result >= 0;
}

bool Socket::has_data(timeval timeout)
{
	fd_set fdset;
	FD_ZERO(&fdset);
	FD_SET(this->handle, &fdset);
	
	int result = select(this->handle + 1, &fdset, NULL, NULL, &timeout);
	
	#ifdef SOCKET_CHECK_ERRORS
	if (result < 0)
	{
		HelpPrinter::print(LOG_ERROR, "couldn't select read data from socket %d, error %d\n", this->handle, errno);
	}
	#endif
	
	if (result < 1)
	{
		return false;
	}
	else return true;
}

bool Socket::has_data()
{
    timeval timeout = { 0, 100 };
	return this->has_data(timeout);
}

bool Socket::is_connected()
{
    return this->connected;
}

int Socket::read(char* buffer, size_t max_length)
{   
	int result = ::read(this->handle, buffer, max_length);
	
	#ifdef SOCKET_CHECK_ERRORS
	if (result < 0)
	{
		HelpPrinter::print(LOG_ERROR, "couldn't read data from socket %d, error %d\n", this->handle, errno);
	}
	#endif
    
    if (result == 0)
    {
        this->disconnect();
    }
	
	return result;
}

int Socket::readline(char* buffer, size_t max_length)
{  
    char* pos = strchr(this->line_buf, '\n');
    
    if (pos != NULL)
    {
        size_t count = (pos - this->line_buf) + 1;    // +1 to skip over newline
        
        memcpy(buffer, this->line_buf, std::min(count, max_length - 1));
        buffer[count] = '\0';
        
        if (count < line_index)
        {
            memcpy(this->line_buf, this->line_buf + count, this->line_index - count);
            this->line_index = this->line_index - count;
        }
        else
        {
            this->line_buf[0] = '\0';
            this->line_index = 0;
        }
        
        return std::min(max_length, count + 1); // +1 because of null termination
    }
    else
    {   
        int read_count = sizeof(this->line_buf) - this->line_index;
        int len = this->read(this->line_buf + this->line_index, read_count);
        
        if (len > 0 && this->is_connected())
        {
            this->line_index += len;
            this->line_buf[this->line_index] = '\0';
        
            return this->readline(buffer, max_length);
        }
        else return 0;
    }
}

void Socket::get_info(char* host, unsigned short* port)
{
	sockaddr_in address;
	uint address_size = sizeof(address);
	
	int result = getsockname(this->handle, (sockaddr*) &address, &address_size);
	
	#ifdef SOCKET_CHECK_ERRORS
	if (result != 0)
	{
		HelpPrinter::print(LOG_ERROR, "couldn't get socket name %s %hu, error %d\n", host, port, errno);
	}
	#endif

	strcpy(host, inet_ntoa(address.sin_addr));
	*port = ntohs(address.sin_port); 
}

void Socket::get_remote_info(char* host, unsigned short* port)
{
	sockaddr_in address;
	uint address_size = sizeof(address);
	
	int result = getpeername(this->handle, (sockaddr*) &address, &address_size);
	
	#ifdef SOCKET_CHECK_ERRORS
	if (result != 0)
	{
		HelpPrinter::print(LOG_ERROR, "couldn't get remote peer info, error %d\n", errno);
	}
	#endif
	
	strcpy(host, inet_ntoa(address.sin_addr));
	*port = ntohs(address.sin_port); 
}

void Socket::close()
{
    this->disconnect();
	::close(this->handle);
}

sockaddr_in Socket::create_address(in_addr addr, unsigned short port)
{
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr = addr;
	
	return address;
}

sockaddr_in Socket::create_address(unsigned short port)
{
	in_addr addr_any = { INADDR_ANY };
	
	return Socket::create_address(addr_any, port);
}
