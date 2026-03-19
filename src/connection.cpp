#include "../headers/connection.hpp"
#include "../headers/clientsCommand.hpp"
#include "../headers/serverService.hpp"
#include "../headers/threadSafety.hpp"

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
    pthread_t server_tid, ui_tid, server_udp_tid;

    // Create the UDP Discovery Responder Thread
    if (pthread_create(&server_udp_tid, NULL, udp_discovery_responder, NULL) != 0)
    {
        perror("Failed to create UDP discovery responder thread");
        return;
    }

    struct server_args s_args;
    s_args.server_fd = server_fd;

    // Create the Server Thread
    if (pthread_create(&server_tid, NULL, server_listener_thread_tcp, &s_args) != 0)
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
    pthread_join(server_udp_tid, NULL);

    close(server_fd);
}
