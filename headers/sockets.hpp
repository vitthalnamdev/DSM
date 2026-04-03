#pragma once
#include <iostream>

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

public:
    Socket();
    Socket(socket_t fd);
    ~Socket();

    bool connect_socket(const char *ip);
    int sendData(const char *data, size_t len);
    int receive(char *buffer, size_t len);
    int sendFile(int filefd, off_t *offset, size_t chunk);
    int acceptConnection(int server_fd);
    void close();

    socket_t getSockfd() const { return sockfd; }

    void setIpFromSockaddr(struct sockaddr_in *addr);
};