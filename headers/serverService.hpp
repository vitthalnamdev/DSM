#ifndef SERVER_SERVICE_H
#define SERVER_SERVICE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

typedef void (*server_command_handler_t)(int);

typedef struct
{
    const char *cmd_name;
    server_command_handler_t handler;
} CommandMapServer;

struct server_args
{
    int server_fd;
};

void *server_listener_thread_tcp(void *args);
void *udp_discovery_responder(void *args);

extern char client_ip[INET_ADDRSTRLEN];

void handle_status_check(int sock);

void handle_connect_server(int sock);

void handle_share_file_server(int sock);

extern int OPEN_RECEIVE_FILE_CONNECTION;

void handle_receive_file_server(int sock);

void handle_close_receive_file_server(int sock);

extern CommandMapServer server_dispatch_table[];

#endif