#include "udp_socket.h"

UdpSocket::UdpSocket()
{
	this->handle = socket(AF_INET, SOCK_DGRAM, 0);
	this->line_index = 0;
	this->line_buf[0] = 0;

	if (this->handle < 0)
	{
		printf("error creating socket %d\n", errno);
	}
}

UdpSocket::~UdpSocket()
{
	this->close();
}

int UdpSocket::get_handle()
{
	return this->handle;
}

void UdpSocket::bind(unsigned short port)
{
	int opt = 1;
	int result = setsockopt(this->handle, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	
	if (result != 0)
	{
		printf("error setting socket options, error %d\n", errno);
	}
	
	const sockaddr_in address = UdpSocket::create_address(port);
	result = ::bind(this->handle, (const sockaddr*) &address, sizeof(address));
	
	if (result != 0)
	{
		printf("error binding socket %d\n", errno);
	}
}

bool UdpSocket::write(const char* host, unsigned short port, const char* string)
{
	return this->write(host, port, string, strlen(string) + 1);
}
bool UdpSocket::write(const char* host, unsigned short port, const char* buffer, size_t length)
{
	hostent *hostip = gethostbyname(host);
	
	sockaddr_in address = UdpSocket::create_address(*(in_addr*) hostip->h_addr_list[0], port);
	
	int result = ::sendto(this->handle, buffer, length, 0, (sockaddr*) &address, sizeof(address));
	
	if (result < 0)
	{
		printf("couldn't send data through socket %d, error %d\n", this->handle, errno);
	}
    
	return result >= 0;
}

bool UdpSocket::has_data(timeval timeout)
{
	fd_set fdset;
	FD_ZERO(&fdset);
	FD_SET(this->handle, &fdset);
	
	int result = select(this->handle + 1, &fdset, NULL, NULL, &timeout);
	
	if (result < 0)
	{
		printf("couldn't select read data from socket %d, error %d\n", this->handle, errno);
	}
	
	if (result < 1)
	{
		return false;
	}
	else return true;
}

bool UdpSocket::has_data()
{
    timeval timeout = { 0, 100 };
	return this->has_data(timeout);
}

int UdpSocket::read(char* buffer, size_t max_length, sockaddr* remote_host)
{   
	sockaddr_in from_addr;
	unsigned int from_len = sizeof(from_addr);
	
	int result = ::recvfrom(this->handle, buffer, max_length, 0, (sockaddr*) &from_addr, &from_len);
	
	*remote_host = *((sockaddr*) &from_addr);
	
	if (result < 0)
	{
		printf("couldn't read data from socket %d, error %d\n", this->handle, errno);
	}
	
	return result;
}

int UdpSocket::readline(char* buffer, size_t max_length, sockaddr* remote_host)
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
        int len = this->read(this->line_buf + this->line_index, read_count, remote_host);
        
        if (len > 0)
        {
            this->line_index += len;
            this->line_buf[this->line_index] = '\0';
        
            return this->readline(buffer, max_length, remote_host);
        }
        else return 0;
    }
}

void UdpSocket::close()
{
	::close(this->handle);
}

sockaddr_in UdpSocket::create_address(in_addr addr, unsigned short port)
{
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr = addr;
	
	return address;
}

sockaddr_in UdpSocket::create_address(unsigned short port)
{
	in_addr addr_any = { INADDR_ANY };
	
	return UdpSocket::create_address(addr_any, port);
}
