#include "connection.h"
#include "common.h"
#include "config.h"
#include <stdio.h>
#include <time.h>

int ConnectToServer()
{
    if (client_socket != INVALID_SOCKET)
    {
        closesocket(client_socket);
        client_socket = INVALID_SOCKET;
    }

    client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket == INVALID_SOCKET)
    {
        return 0;
    }

    u_long iMode = 1;
    ioctlsocket(client_socket, FIONBIO, &iMode);

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(SERVER_PORT);

    int connectResult = connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (connectResult == SOCKET_ERROR)
    {
        if (WSAGetLastError() != WSAEWOULDBLOCK)
        {
            closesocket(client_socket);
            client_socket = INVALID_SOCKET;
            return 0;
        }

        fd_set writefds;
        FD_ZERO(&writefds);
        FD_SET(client_socket, &writefds);

        struct timeval timeout;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        int selectResult = select(0, NULL, &writefds, NULL, &timeout);
        if (selectResult <= 0)
        {
            closesocket(client_socket);
            client_socket = INVALID_SOCKET;
            return 0;
        }
    }

    iMode = 0;
    ioctlsocket(client_socket, FIONBIO, &iMode);

    bConnected = TRUE;
    return 1;
}

int SendDataToServer(const char *data)
{
    if (!bConnected)
    {
        if (!ConnectToServer())
        {
            return 0;
        }
    }

    int result = send(client_socket, data, strlen(data), 0);
    if (result == SOCKET_ERROR)
    {
        bConnected = FALSE;
        closesocket(client_socket);
        client_socket = INVALID_SOCKET;
        return 0;
    }
    return 1;
}

void SendClientInfo()
{
    char buffer[1024];
    time_t now;
    time(&now);

    sprintf(buffer, "INFO|%s|%s|%lld", computer_name, user_name, (long long)now);
    SendDataToServer(buffer);
}