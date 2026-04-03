#include "../../headers/serverService.hpp"
#include "../../headers/sockets.hpp"

void *handle_connect_server(void *arg)
{
    Socket socket = *(Socket *)arg;
    char connect_msg = '1';
    socket.sendData(&connect_msg, sizeof(connect_msg));
    delete (Socket *)arg; 
    return NULL;
}