#include "../../headers/serverService.hpp"

void *handle_connect_server(void *arg)
{
    int sock = *(int *)arg;
    char connect_msg = '1';
    send(sock, &connect_msg, sizeof(connect_msg), 0);
    close(sock);
    return NULL;
}