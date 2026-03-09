#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include "../headers/connection.h"
#include "../headers/clientsCommand.h"
#include "../headers/serverService.h"
#include "../headers/threadSafety.h"


// --- THREAD 1: Server Listener ---
void *server_listener_thread(void *args)
{  
    struct server_args *s_args = (struct server_args *)args;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int new_socket;

    while (1)
    {
        new_socket = accept(s_args->server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);

        if (new_socket < 0)
        {
            perror("accept failed");
            continue;
        }


        

        // 1. Create a buffer to hold the incoming message
        char buffer[1024] = {0};

        // 2. Read the message sent by the client
        // recv() returns the number of bytes received
        ssize_t bytes_read = recv(new_socket, buffer, sizeof(buffer) - 1, 0);

        if (bytes_read > 0)
        {
            // Null-terminate the string and clean up newlines
            buffer[bytes_read] = '\0';
            buffer[strcspn(buffer, "\r\n")] = 0;

            printf("Received from client: [%s]\n", buffer);

            // 3. Dispatch Logic: Execute code based on the command
            int found = 0;
            for (int i = 0; dispatch_table[i].cmd_name != NULL; i++)
            {
                if (strcmp(buffer, dispatch_table[i].cmd_name) == 0)
                {
                    dispatch_table[i].handler(new_socket);
                    wait(); // Wait for the command thread to signal that it's done processing
                    found = 1;
                    break;
                }
            }

            if (!found)
            {
                char *err = "Unknown Command\n";
                send(new_socket, err, strlen(err), 0);
            }
        }

        // pthread_mutex_unlock(&mesh_info.lock);

        // 4. IMPORTANT: Close the socket ONLY after you are done responding

        close(new_socket);
    }
    return NULL;
}

void connection()
{
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;

    // 1. Setup Socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // 2. Thread Management
    pthread_t server_tid, ui_tid;
    struct server_args s_args;
    s_args.server_fd = server_fd;

    // Create the Server Thread
    if (pthread_create(&server_tid, NULL, server_listener_thread, &s_args) != 0)
    {
        perror("Failed to create server thread");
        return;
    }

    // Create the UI/Command Thread

    struct commands_args c_args;

    c_args.mesh_info = &mesh_info;

    if (pthread_create(&ui_tid, NULL, commands, &c_args) != 0)
    {
        perror("Failed to create UI thread");
        return;
    }

    // Wait for threads to finish (which they won't in this infinite loop)
    pthread_join(server_tid, NULL);
    pthread_join(ui_tid, NULL);

    close(server_fd);
}
