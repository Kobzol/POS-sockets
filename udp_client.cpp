#include <iostream>

#include "udp_socket.h"

int main(int argc, char** argv)
{
	UdpSocket client;
	client.bind(8002);
	
	const char* message = "ahoj\n";
	
	//client.write("224.3.29.71", 8001, message); multicast
	
	/*int broadcastEnable=1;
	int ret = setsockopt(client.get_handle(), SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
	
	client.write("255.255.255.255", 8001, message); // broadcast
	*/
	 
	printf("Sending to server: %s", message);
	client.write("localhost", 8001, message);
	
	sockaddr_in from_addr;
	unsigned int from_len;
	char buf[1024];
	
	int size = client.readline(buf, sizeof(buf), (sockaddr*) &from_addr);
	buf[size] = '\0';
	
	printf("Received from server: %s", buf);
	
	client.close();
	
	return 0;
}
