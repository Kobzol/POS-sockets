all: tcp_server tcp_client udp_client udp_server

tcp_server: tcp_server.cpp socket.cpp server_socket.cpp
	g++ -O0 $^ -otcp_server
	
tcp_client: tcp_client.cpp socket.cpp server_socket.cpp
	g++ -O0 $^ -otcp_client

udp_client: udp_client.cpp udp_socket.cpp
	g++ -O0 $^ -oudp_client
	
udp_server: udp_server.cpp udp_socket.cpp
	g++ -O0 $^ -oudp_server

clean:
	rm tcp_server tcp_client udp_client udp_server
