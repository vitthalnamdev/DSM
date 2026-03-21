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

void handle_receive_file(int sock)
{
    printf("Press Q to exit.\n");

    // Register cleanup mechanisms
    signal(SIGINT, handle_sigint);
    atexit(cleanup);

    const char command[16] = "receiveFile";

    char *result = sendToServer(sock, command, SELFIP);

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
                        printf("\nExiting...\n");

                        // TODO: notify server to stop sending file
                        // send(sock, "STOP", 4, 0);

                        break;
                    }
                }
            }

            /* ---------------- File Receive Logic ---------------- */
            // Example placeholder:
            // char buffer[4096];
            // int bytes = recv(sock, buffer, sizeof(buffer), 0);
            // if (bytes > 0) {
            //     write(file_fd, buffer, bytes);
            // }
            // else if (bytes == 0) {
            //     printf("\nFile transfer complete.\n");
            //     break;
            // }
            // else {
            //     perror("recv");
            //     break;
            // }
        }

        disable_raw_mode(); // restore terminal on normal exit
    }
    else
    {
        printf("Failed to receive file: %s\n", result);
    }

    free(result);
}