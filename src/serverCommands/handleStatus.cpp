#include "../../headers/serverService.hpp"
#include "../../headers/Status_codes.hpp"

void *handle_status_check(void *arg)
{
    int sock = *(int *)arg;
    if(OPEN_RECEIVE_FILE_CONNECTION)
    {
        send(sock, STATUS_MESSAGES[OPEN_SHAREFILE_CONNECTION], strlen(STATUS_MESSAGES[OPEN_SHAREFILE_CONNECTION]), 0);
    }
    else
    {
        send(sock, STATUS_MESSAGES[SUCCESS], strlen(STATUS_MESSAGES[SUCCESS]), 0);
    }
    close(sock);
    return NULL;
}
