#include "../../headers/clientsCommand.h"

void handleStatus(int sock)
{
    char *SERVER_IP = "127.0.0.1"; // Assuming server is on localhost for testing
    int result = sendToServer("status", SERVER_IP);
    if (result)
    {
        printf("Server is active and responded to status check.\n");
    }
    else
    {
        printf("Failed to connect to server or server did not respond.\n");
    }
}