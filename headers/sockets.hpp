// sockets.hpp
#include <iostream>
#pragma once

class Socket
{
private:
    using socket_t =
#if defined(_WIN32) || defined(_WIN64)
        SOCKET;
#else
        int;
#endif

    socket_t sockfd;
    char *myIp;

public:
    Socket();
    ~Socket();

    bool connect_socket(const char *ip);
    int sendData(const char *data, size_t len);
    int receive(char *buffer, size_t len);
    void close();
    socket_t getSockfd() const { return sockfd; }
    const char *getMyIp() const { return myIp; }
};