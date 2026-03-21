#include "../../headers/clientsCommand.hpp"
#include "../../headers/Status_codes.hpp"

struct termios orig_termios;

/* ---------------- Terminal Handling ---------------- */

void disable_raw_mode()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}

void enable_raw_mode()
{
    tcgetattr(STDIN_FILENO, &orig_termios);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

/* ---------------- Cleanup Handlers ---------------- */

void cleanup()
{
    disable_raw_mode();
}

void handle_sigint(int sig)
{
    (void)sig;
    disable_raw_mode();
    printf("\nTerminal restored. Exiting...\n");
    exit(0);
}

/* ---------------- Main Logic ---------------- */

void handle_receive_file()
{
    printf("Press Q to exit.\n");

    // Register cleanup mechanisms
    signal(SIGINT, handle_sigint);
    atexit(cleanup);

    char command[30] = "receiveFile";

    char *result = sendToServer(command, SELFIP);

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
                ssize_t n = read(STDIN_FILENO, &ch, 1);

                if (n > 0)
                {
                    if (ch == 'Q' || ch == 'q')
                    {
                        strcpy(command, "closeReceiveFile");
                        result = sendToServer(command, SELFIP);
                        if (strcmp(result, STATUS_MESSAGES[SUCCESS]) == 0)
                        {
                            printf("\nExiting...\n");
                        }
                        else
                        {
                            exit(-1);
                        }
                        break;
                    }
                }
            }
        }

        disable_raw_mode(); // restore terminal on normal exit
    }
    else
    {
        printf("Failed to receive file: %s\n", result);
    }

    free(result);
}