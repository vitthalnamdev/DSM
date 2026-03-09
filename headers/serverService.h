#ifndef SERVER_SERVICE_H
#define SERVER_SERVICE_H

typedef void (*command_handler_t)(int);

typedef struct
{
    const char *cmd_name;
    command_handler_t handler;
} CommandMapServer;

void handle_status_check(int sock);

void handle_exit(int sock);

extern CommandMapServer dispatch_table[];

#endif