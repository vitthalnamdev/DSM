#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "../headers/serverService.h"


CommandMapServer server_dispatch_table[] = {
    {"status", handle_status_check},  
    {"connect", handle_connect_server},
    {NULL, NULL} // Sentinel value to mark the end
};  



void handle_connect_server(int sock)
{
    char connect_msg = '1';
    send(sock, &connect_msg, sizeof(connect_msg), 0);
}


void handle_status_check(int sock)
{
    char status = '1';
    send(sock, &status, sizeof(status), 0);
}


