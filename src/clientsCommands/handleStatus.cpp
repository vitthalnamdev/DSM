#include "../../headers/clientsCommand.hpp"

char *askStatus(int sock, const char *SERVER_IP)
{
    const char *statuscommand = "status";
    char *response = sendToServer(sock, statuscommand, SERVER_IP);
    return response;
}

void handleStatus(int sock)
{

    char *response = askStatus(sock, SELFIP);
    if (response)
    {
        printf("Server response: %s\n", response);
        free(response);
    }
    else
    {
        printf("Failed to connect to server or server did not respond.\n");
    }
}