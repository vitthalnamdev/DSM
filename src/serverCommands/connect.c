#include "../../headers/serverService.h"

void handle_connect_server(int sock)
{
    char connect_msg = '1';
    send(sock, &connect_msg, sizeof(connect_msg), 0);
}