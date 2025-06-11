#include "config.h"
#include "monitor.h"
#include "common.h"
#include "connection.h"
#include <stdio.h>

DWORD WINAPI ConnectionMonitorThread(LPVOID lpParam)
{
    while (!bShouldExit)
    {
        if (!bConnected)
        {
            printf("Attempting to connect to server...\n");
            if (ConnectToServer())
            {
                printf("Connected to server successfully.\n");
                SendClientInfo();
            }
            else
            {
                printf("Connection failed. Retrying in %d seconds...\n", RECONNECT_INTERVAL / 1000);
            }
        }
        Sleep(RECONNECT_INTERVAL);
    }
    return 0;
}