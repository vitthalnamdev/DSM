#ifdef __linux__

#include "../../headers/sockets.hpp"
#include "../../headers/clientsCommand.hpp"
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

Socket::Socket()
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
    }
}

bool Socket::connect_socket(const char *ip)
{
    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, ip, &addr.sin_addr) <= 0)
    {
        perror("inet_pton");
        return false;
    }

    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("connect");
        return false;
    }
    myIp = (inet_ntoa(addr.sin_addr));
    return true;
}

int Socket::sendData(const char *data, size_t len)
{
    return send(sockfd, data, len, 0);
}

int Socket::receive(char *buffer, size_t len)
{
    return recv(sockfd, buffer, len, 0);
}

void Socket::close()
{
    ::close(sockfd);
}

Socket::~Socket()
{
    close();
}

#endif