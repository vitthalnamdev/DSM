#include "../../headers/serverService.hpp"
#include "../../headers/Status_codes.hpp"

void *handle_distributive_systems(void *arg)
{
    int sock = *(int *)arg;
    // This function will handle the distributive computing request from the client.
    // It will receive the code file and the train file from the client, and then it will execute the code on the server and send the result back to the client.
    OPEN_RECEIVE_FILE_CONNECTION = 1; // Set the flag to indicate that we are ready to receive files. 
    
    send(sock, STATUS_MESSAGES[OPEN_SHAREFILE_CONNECTION], strlen(STATUS_MESSAGES[OPEN_SHAREFILE_CONNECTION]), 0);
    


    return NULL;
}