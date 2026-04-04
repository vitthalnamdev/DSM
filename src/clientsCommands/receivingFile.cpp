#include "../../headers/clientsCommand.hpp"
#include "../../headers/Status_codes.hpp"

void handle_receive_file()
{
    printf("Press Q to exit.\n");

    char command[30] = "receiveFile";

    char *result = sendToServer(command, SELFIP);

    if (strcmp(result, STATUS_MESSAGES[OPEN_SHAREFILE_CONNECTION]) == 0)
    {
        printf("Waiting to receive file from server...\n");

        while (1)
        {

            // if (activity > 0 && FD_ISSET(STDIN_FILENO, &readfds))
            // {
            //     char ch;
            //     ssize_t n = read(STDIN_FILENO, &ch, 1);

            //     if (n > 0)
            //     {
            //         if (ch == 'Q' || ch == 'q')
            //         {
            //             strcpy(command, "closeReceiveFile");
            //             result = sendToServer(command, SELFIP);
            //             if (strcmp(result, STATUS_MESSAGES[SUCCESS]) == 0)
            //             {
            //                 printf("\nExiting...\n");
            //             }
            //             else
            //             {
            //                 clear_stdin();
            //                 exit(-1);
            //             }
            //             break;
            //         }
            //     }
            // }
        }
    }
    else
    {
        printf("Failed to receive file: %s\n", result);
    }

    clear_stdin();

    free(result);
}