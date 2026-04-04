#include <stdio.h>
#include "headers/connection.hpp"
#include "headers/sockets.hpp"

int main()
{
// #ifdef defined(WIN_32) || defined(WIN_64)
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
// #endif
    connection();
// #ifdef defined(WIN_32) || defined(WIN_64)
    WSACleanup();
// #endif

    return 0;
}