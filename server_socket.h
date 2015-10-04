#pragma once

#include "socket.h"

class ServerSocket : public Socket {
public:
	ServerSocket();
	~ServerSocket();

	bool listen(int backlog);
	Socket* accept();
};
