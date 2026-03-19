#ifndef CONNECTION_H
#define CONNECTION_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

struct server_args {
    int server_fd;
};

void* server_listener_thread_tcp(void* args);
void* udp_discovery_responder(void* args);

void connection();

#endif