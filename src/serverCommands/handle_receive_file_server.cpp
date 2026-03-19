#include "../../headers/serverService.hpp"
#include "../../headers/Status_codes.hpp"

void handle_receive_file_server(int sock) {
    // Set the flag to indicate that a receive file connection is open
    OPEN_RECEIVE_FILE_CONNECTION = 1;
    // Send a response to the client indicating that the connection is open
    send(sock, STATUS_MESSAGES[OPEN_SHAREFILE_CONNECTION], strlen(STATUS_MESSAGES[OPEN_SHAREFILE_CONNECTION]), 0);
}