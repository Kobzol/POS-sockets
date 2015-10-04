#include <iostream>

#include "socket.h"

int main(int argc, char** argv)
{
	Socket client;
	client.connect("localhost", 8000);
	client.write("ahoj\n");
	
	client.close();
	
	return 0;
}
