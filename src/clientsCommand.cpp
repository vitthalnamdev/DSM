#include "../headers/clientsCommand.hpp"
#include "../headers/threadSafety.hpp"
#include "../headers/Status_codes.hpp"

CommandMapClient client_dispatch_table[] = {
    {"listConnections", handle_list_connections},
    {"status", handleStatus},
    {"exit", handle_exit},
    {"connect", handle_connect_client},
    {"sendFile", handle_send_file},
    {"receiveFile", handle_receive_file},
    {NULL, NULL} // Sentinel value to mark the end
};

char buffer[BUFFER_SIZE] = {0};
char command[BUFFER_SIZE] = {0};

char **ip_list = NULL;
char **ip_status = NULL;
const char *SELFIP = "127.0.0.1";


char *sendToServer(int sock, const char *command, const char *SERVER_IP)
{
    connect_socket(sock, SERVER_IP);

    send(sock, command, strlen(command), 0);

    int valread = read(sock, buffer, sizeof(buffer));

    if (valread > 0)
    {
        buffer[valread] = '\0';

        char *result = (char *)malloc(valread + 1); // +1 for '\0'
        if (!result)
            return NULL;

        memcpy(result, buffer, valread + 1); // copy only valid bytes
        return result;
    }
    else
    {
        char *err = (char *)STATUS_MESSAGES[CONNECTION_ERROR];
        return err;
    }
}

int create_socket(void)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("socket");
        return -1;
    }
    return sock;
}

int connect_socket(int sock, const char *ip)
{
    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, ip, &addr.sin_addr) <= 0)
    {
        perror("inet_pton");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("connect");
        return -1;
    }

    return 0;
}

void *commands(void *args)
{

    SharedData *mesh_info = ((struct commands_args *)args)->mesh_info;

    while (1)
    {
        printf(ANSI_COLOR_GREEN "\n> " ANSI_COLOR_RESET);
        fflush(stdout); // Ensure prompt shows up

        if (fgets(command, sizeof(command), stdin) != NULL)
        {
            command[strcspn(command, "\n")] = 0; // Remove newline

            int found = 0;
            for (int i = 0; client_dispatch_table[i].cmd_name != NULL; i++)
            {
                if (strcmp(command, client_dispatch_table[i].cmd_name) == 0)
                {
                    int sock = create_socket();
                    if (sock < 0)
                    {
                        printf("Failed to create socket\n");
                        break;
                    }
                    client_dispatch_table[i].handler(sock);
                    found = 1;
                    close(sock);
                    break;
                }
            }

            if (!found)
            {
                printf("Unknown Command\n");
            }
        }
    }
    return NULL;
}