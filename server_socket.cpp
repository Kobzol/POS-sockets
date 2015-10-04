#include "server_socket.h"

ServerSocket::ServerSocket()
{
	
}

ServerSocket::~ServerSocket()
{
	
}

bool ServerSocket::listen(int backlog)
{	
	int result = ::listen(this->get_handle(), backlog);
	
	if (result != 0)
	{
		printf("couldn't start listening, error %d\n", errno);
	}
	
	return result == 0;
}

Socket* ServerSocket::accept()
{
	int client = ::accept(this->get_handle(), NULL, NULL);
	return new Socket(client);
}
