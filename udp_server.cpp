#include <iostream>

#include "udp_socket.h"

int main(int argc, char** argv)
{
	UdpSocket server;
	server.bind(8001);
	
	/*
	Multicast
	
	u_char loop = 1;
	setsockopt(server.get_handle(), IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop));
	
	u_char ttl = 255;
	setsockopt(server.get_handle(), IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));
	
	struct ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = inet_addr("224.3.29.71");
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	
	setsockopt(server.get_handle(), IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));*/
	
	sockaddr_in from_addr;
	unsigned int from_len;
	char buf[1024];
	
	int size = server.readline(buf, sizeof(buf), (sockaddr*) &from_addr);
	
	buf[size] = '\0';
	
	printf("Received from client: %s", buf);
	
	char* host = inet_ntoa(from_addr.sin_addr);
	unsigned short port = ntohs(from_addr.sin_port);
	
	printf("Sending back to %s:%d\n", host, port);
	
	server.write(host, port, buf);
	
	server.close();
	
	return 0;
}
