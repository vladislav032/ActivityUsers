#include "ServerApp.h"

// функция для работы server в отдельном потоке
DWORD WINAPI ServerThread(LPVOID lpParam)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        MessageBox(hMainWindow, L"Ошибка инициализации Winsock", L"Ошибка", MB_OK | MB_ICONERROR);
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET)
    {
        MessageBox(hMainWindow, L"Ошибка создания сокета", L"Ошибка", MB_OK | MB_ICONERROR);
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(SERVER_PORT);

    if (bind(serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        MessageBox(hMainWindow, L"Ошибка привязки сокета", L"Ошибка", MB_OK | MB_ICONERROR);
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        MessageBox(hMainWindow, L"Ошибка прослушивания", L"Ошибка", MB_OK | MB_ICONERROR);
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    while (serverRunning)
    {
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);

        SOCKET clientSocket = accept(serverSocket, (sockaddr *)&clientAddr, &clientAddrSize);
        if (clientSocket == INVALID_SOCKET)
        {
            if (serverRunning)
            {
                MessageBox(hMainWindow, L"Ошибка принятия подключения", L"Ошибка", MB_OK | MB_ICONERROR);
            }
            continue;
        }

        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);

        wchar_t wClientIP[INET_ADDRSTRLEN];
        MultiByteToWideChar(CP_ACP, 0, clientIP, -1, wClientIP, INET_ADDRSTRLEN);

        wchar_t msg[256];
        swprintf(msg, 256, L"Новое подключение: %s", wClientIP);
        MessageBox(hMainWindow, msg, L"Новое подключение", MB_OK | MB_ICONINFORMATION);

        std::thread(HandleClient, clientSocket, std::wstring(wClientIP)).detach();
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
