#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "../headers/clientsCommand.h"
#include "../headers/threadSafety.h"

void sendToServer(char *command)
{
    int sock = 0, valread;
    struct sockaddr_in serv_addr;

    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);

    char *SERVER_IP = "127.0.0.1";

    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return;
    }

    // Connect with error checking
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed. Is the server running?\n");
        return;
    }

    send(sock, command, strlen(command), 0);

    valread = read(sock, buffer, sizeof(buffer));

    if (valread > 0)
    {
        printf("Message from server: %s\n", buffer);
        resume();
    }
    else
    {
        printf("Failed to read or connection closed by server\n");
    }

    close(sock);
}

void *commands(void *args)
{
    char buffer[1024];

    SharedData *mesh_info = ((struct commands_args *)args)->mesh_info;

    while (1)
    {
        printf("> ");
        fflush(stdout); // Ensure prompt shows up

        if (fgets(buffer, sizeof(buffer), stdin) != NULL)
        {
            sendToServer(buffer);
        }
    }
    return NULL;
}