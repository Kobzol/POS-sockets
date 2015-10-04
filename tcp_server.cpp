#include <iostream>

#include "server_socket.h"

int main(int argc, char** argv)
{
	ServerSocket server;
	server.bind(8000);
	server.listen(1);
	
	Socket* client = server.accept();
	
	char msg[1024];
	client->readline(msg, sizeof(msg));
	 
	printf("%s\n", msg);
	
	client->close();
	
	delete client;
	
	server.close();
	
	return 0;
}
