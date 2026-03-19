#include "../../headers/clientsCommand.hpp"
#include "../../headers/Status_codes.hpp"

struct termios orig_termios;

void enable_raw_mode()
{
    tcgetattr(STDIN_FILENO, &orig_termios);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

void disable_raw_mode()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}

void handle_receive_file(int sock)
{
    printf("Press Q to exit.\n");

    char *result = sendToServer(sock, "receiveFile", SELFIP);

    if (strcmp(result, STATUS_MESSAGES[OPEN_SHAREFILE_CONNECTION]) == 0)
    {
        printf("Waiting to receive file from server...\n");

        enable_raw_mode();  

        while (1)
        {
            fd_set readfds;
            struct timeval tv;

            FD_ZERO(&readfds);
            FD_SET(STDIN_FILENO, &readfds);

            tv.tv_sec = 1;
            tv.tv_usec = 0;

            int activity = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);

            if (activity > 0 && FD_ISSET(STDIN_FILENO, &readfds))
            {
                char ch;
                read(STDIN_FILENO, &ch, 1);  

                if (ch == 'Q' || ch == 'q')
                {
                    printf("\nExiting...\n");
                    // send a message to server to stop sending file.
                    break;
                }
            }

            // 👉 recv(sock, buffer, size, 0);
        }

        disable_raw_mode(); // restore terminal
    }
    else
    {
        printf("Failed to receive file: %s\n", result);
    }

    free(result);
}