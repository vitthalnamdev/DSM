#include "../../headers/clientsCommand.hpp"
#include "../../headers/shareFile.hpp"
#include "../../headers/Status_codes.hpp"

void handle_send_file(int sock)
{
    // TODO: [Implementation] If, only for sending file , or this function is called, Then I have to show through UI (the percentage of file sent) , and also the file name that is being sent, and the I.P to which it is being sent.
    printf("Enter the I.P that you want to share the file with: ");
    char IP[16];
    if (!fgets(IP, sizeof(IP), stdin))
    {
        perror("Failed to read IP address");
        close(sock);
        return;
    }

    IP[strcspn(IP, "\n")] = 0;

    const char *command = "shareFile";
    uint32_t cmdlen = strlen(command);

    if (!send_all_sync(sock, command, cmdlen))
    {
        perror("Failed to send command");
        close(sock);
        return;
    }

    char response[128];
    int valread = read(sock, response, sizeof(response) - 1);
    response[valread] = '\0';

    if (strcmp(response, STATUS_MESSAGES[SUCCESS]) != 0)
    {
        printf("Server response: %s\n", response);
        printf("Cannot share file with %s. Reason: %s\n", IP, response);
        close(sock);
        return;
    }

    printf("Enter the file address that you want to share: ");
    char filename[256];
    if (!fgets(filename, sizeof(filename), stdin))
    {
        perror("Failed to read filename");
        close(sock);
        return;
    }

    filename[strcspn(filename, "\n")] = 0;

    printf("Sharing file %s with %s...\n", filename, IP);

    int file_sent = send_file(sock, filename, IP);

    if (!file_sent)
    {
        printf("Failed to share file with %s\n", IP);
        close(sock);
        return;
    }

    valread = read(sock, response, sizeof(response) - 1);

    if (valread > 0)
    {
        response[valread] = '\0';
        printf("Server response: %s\n", response);
        printf("File shared successfully with %s\n", IP);
    }
    else
    {
        printf("Failed to share file with %s\n", IP);
    }

    close(sock);
}