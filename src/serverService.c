#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "../headers/serverService.h"


CommandMapServer dispatch_table[] = {
    {"status", handle_status_check},
    {"exit", handle_exit},
    {NULL, NULL} // Sentinel value to mark the end
};



void handle_status_check(int sock)
{
    char *status_message = "[Server] Connected to client:\n";
    send(sock, status_message, strlen(status_message), 0);
}


void handle_exit(int sock){
    exit(0);
}
