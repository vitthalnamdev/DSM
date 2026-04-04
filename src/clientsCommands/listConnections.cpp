#include "../../headers/clientsCommand.hpp"
#include "../../headers/sockets.hpp"

void sendRequest()
{
    int sock;
    struct sockaddr_in broadcast_addr;
    int broadcast_enable = 1;

    // 1. Create UDP Socket (SOCK_DGRAM)
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket creation failed");
        return;
    }

    // 2. Enable Broadcast permission on the socket
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (const char*)&broadcast_enable, sizeof(broadcast_enable)) < 0)
    {
        perror("Setsockopt (SO_BROADCAST) failed");
        close(sock);
        return;
    }

    // 3. Set a Timeout (So we don't wait forever if no one responds)
    int timeout = 1000; // Wait 1 second for replies (in milliseconds)
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) < 0)
    {
        perror("Setsockopt (SO_RCVTIMEO) failed");
    }

    // 4. Configure Broadcast Address
    memset(&broadcast_addr, 0, sizeof(broadcast_addr));
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_port = htons(PORT);
    broadcast_addr.sin_addr.s_addr = inet_addr("255.255.255.255");

    snprintf(command, sizeof(command), "status");

    sendto(sock, command, strlen(command), 0, (struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr));

    // 6. Collect all responses
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);
    int found_count = 0;

    ip_list = (char **)malloc(MAX_IPS * sizeof(char *));
    ip_status = (char **)malloc(MAX_IPS * sizeof(char *));

    if (!ip_list || !ip_status)
    {
        perror("Memory allocation failed");
        close(sock);
        free(ip_list);
        free(ip_status);
        return;
    }

    int count = 0;

    while (1)
    {
        int valread = recvfrom(sock, buffer, sizeof(buffer) - 1, 0,
                               (struct sockaddr *)&server_addr, &addr_len);

        if (valread < 0)
            break;

        if (count >= MAX_IPS)
        {
            printf("Max IP limit reached\n");
            break;
        }

        buffer[valread] = '\0';

        char *receivedStatus = (char *)malloc(valread + 1);
        if (!receivedStatus)
            break;

        memcpy(receivedStatus, buffer, valread + 1);

        char *ip = (char *)malloc(IP_LENGTH);
        if (!ip)
        {
            free(receivedStatus);
            break;
        }

        strncpy(ip, inet_ntoa(server_addr.sin_addr), IP_LENGTH);
        ip[IP_LENGTH - 1] = '\0';

        ip_status[count] = receivedStatus;
        ip_list[count] = ip;
        count++;
    }

    close(sock);

    // ✅ NULL terminate safely
    if (count < MAX_IPS)
    {
        ip_list[count] = NULL;
        ip_status[count] = NULL;
    }
}

void handle_list_connections()
{
    sendRequest();

    if (!ip_list)
    {
        printf("Failed to retrieve server IPs.\n");
        return;
    }

    printf("Available servers:\n");
    for (int i = 0; ip_list[i] != NULL; i++)
    {
        printf("%s\n", ip_list[i]);
        free(ip_list[i]);   // Free each IP string after use
        free(ip_status[i]); // Free each status string after use
    }
    free(ip_list);   // Free the list itself
    free(ip_status); // Free the status list itself
}