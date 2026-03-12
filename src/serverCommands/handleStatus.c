#include "../../headers/serverService.h"


void handle_status_check(int sock)
{
    char status = '1';
    send(sock, &status, sizeof(status), 0);
}
