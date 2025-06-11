#pragma once
#include "common.h"
#include "ClientInfo.h"

// глобальные переменные
extern std::vector<ClientInfo> clients;
extern std::recursive_mutex clientsMutex;
extern std::atomic<bool> serverRunning;
extern HWND hListView;
extern HWND hMainWindow;

// функции
void UpdateClientList();
void RequestScreenshot(int index);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
DWORD WINAPI ServerThread(LPVOID lpParam);
void HandleClient(SOCKET clientSocket, std::wstring clientIP);
std::wstring GetClientKey(const ClientInfo &client);