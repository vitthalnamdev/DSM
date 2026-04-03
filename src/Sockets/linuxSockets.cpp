#ifdef __linux__

#include "../../headers/sockets.hpp"
#include "../../headers/clientsCommand.hpp"
#include "../../headers/serverService.hpp"
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sys/sendfile.h>

char client_ip[INET_ADDRSTRLEN] = {0};

Socket::Socket()
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
    }
}

Socket::Socket(socket_t fd)
{
    sockfd = fd;
    if (sockfd < 0)
    {
        perror("socket");
    }
}

void Socket::setIpFromSockaddr(struct sockaddr_in *address)
{
    inet_ntop(AF_INET, &address->sin_addr, client_ip, INET_ADDRSTRLEN);
}

int Socket::acceptConnection(int server_fd)
{
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);

    sockfd = accept(server_fd, (struct sockaddr *)&address, &addrlen);
    if (sockfd < 0)
    {
        perror("accept");
        return -1;
    }
    setIpFromSockaddr(&address);
    return 1;
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

int Socket::sendFile(int filefd, off_t *offset, size_t chunk)
{
    int sent = sendfile(sockfd, filefd, offset, chunk);
    return sent;
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