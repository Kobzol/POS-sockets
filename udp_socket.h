#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>

class UdpSocket {
private:
    int handle;
    char line_buf[1024];
    int line_index;

    static sockaddr_in create_address(in_addr address, unsigned short port);
    static sockaddr_in create_address(unsigned short port);

public:
    UdpSocket();
    ~UdpSocket();

    int get_handle();

    void bind(unsigned short port);

    bool write(const char* host, unsigned short port, const char* string);
    bool write(const char* host, unsigned short port, const char* buffer, size_t length);

    bool has_data();
    bool has_data(timeval timeout);

    int read(char* buffer, size_t max_length, sockaddr* remote_host);
    int readline(char* buffer, size_t max_length, sockaddr* remote_host);

    void close();
};
