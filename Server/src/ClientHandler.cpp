#include "ServerApp.h"

// основаная информация клиента
void HandleClient(SOCKET clientSocket, std::wstring clientIP)
{
    char buffer[BUFFER_SIZE];
    ClientInfo client{clientSocket, L"", L"", clientIP, time(nullptr), false};

    {
        std::lock_guard<std::recursive_mutex> lock(clientsMutex);
        clients.push_back(client);
    }

    while (serverRunning)
    {
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
        if (bytesReceived <= 0)
        {
            break;
        }

        buffer[bytesReceived] = '\0';
        std::lock_guard<std::recursive_mutex> lock(clientsMutex);

        for (auto &c : clients)
        {
            if (c.socket == clientSocket)
            {
                c.lastActivity = time(nullptr);
                break;
            }
        }

        if (strncmp(buffer, "INFO|", 5) == 0)
        {
            char *token = strtok(buffer, "|");
            token = strtok(NULL, "|");
            std::wstring computerName(token, token + strlen(token));
            token = strtok(NULL, "|");
            std::wstring userName(token, token + strlen(token));

            for (auto &c : clients)
            {
                if (c.socket == clientSocket)
                {
                    c.computerName = computerName;
                    c.userName = userName;
                    break;
                }
            }
            PostMessage(hMainWindow, WM_UPDATE_LIST, 0, 0);
        }
        else if (strncmp(buffer, "SCREENSHOT|", 11) == 0)
        {
            char *token = strtok(buffer, "|");
            token = strtok(NULL, "|");
            std::string compName = token;
            token = strtok(NULL, "|");
            std::string user = token;
            token = strtok(NULL, "|");

            int width, height;
            sscanf(token, "%dx%d", &width, &height);

            int headerLen = strlen("SCREENSHOT|") + compName.length() + user.length() + strlen(token) + 3;
            int remainingData = bytesReceived - headerLen;

            char filename[256];
            sprintf(filename, "screenshot_%s_%lld.bmp", compName.c_str(), (long long)time(nullptr));

            FILE *file = fopen(filename, "wb");
            if (!file)
            {
                printf("Не удалось создать файл: %s\n", filename);
                continue;
            }

            BITMAPFILEHEADER bmfh = {0};
            BITMAPINFOHEADER bmih = {0};

            bmfh.bfType = 0x4D42;
            bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
            bmih.biSize = sizeof(BITMAPINFOHEADER);
            bmih.biWidth = width;
            bmih.biHeight = height;
            bmih.biPlanes = 1;
            bmih.biBitCount = 24;
            bmih.biCompression = BI_RGB;

            DWORD imageSize = ((width * 24 + 31) / 32) * 4 * height;
            bmfh.bfSize = bmfh.bfOffBits + imageSize;

            fwrite(&bmfh, 1, sizeof(bmfh), file);
            fwrite(&bmih, 1, sizeof(bmih), file);

            if (remainingData > 0)
            {
                fwrite(buffer + headerLen, 1, remainingData, file);
            }

            int totalReceived = remainingData;
            char *imgBuffer = new char[4096];

            while (totalReceived < imageSize)
            {
                int bytesToReceive = std::min<unsigned long>(4096, imageSize - totalReceived);
                int bytesReceived = recv(clientSocket, imgBuffer, bytesToReceive, 0);
                if (bytesReceived <= 0)
                {
                    break;
                }
                fwrite(imgBuffer, 1, bytesReceived, file);
                totalReceived += bytesReceived;
            }

            delete[] imgBuffer;
            fclose(file);

            if (totalReceived >= imageSize)
            {
                printf("Скриншот сохранен: %s (%d байт)\n", filename, totalReceived + sizeof(bmfh) + sizeof(bmih));
            }
            else
            {
                printf("Не удалось получить полный скриншот: %s (получено %d из %d байт)\n",
                       filename, totalReceived, imageSize);
                remove(filename);
            }
        }
    }

    std::lock_guard<std::recursive_mutex> lock(clientsMutex);
    clients.erase(std::remove_if(clients.begin(), clients.end(),
                                 [clientSocket](const ClientInfo &c)
                                 { return c.socket == clientSocket; }),
                  clients.end());

    closesocket(clientSocket);
    PostMessage(hMainWindow, WM_UPDATE_LIST, 0, 0);
}

// полчение ключа клиента
std::wstring GetClientKey(const ClientInfo &client)
{
    return client.ipAddress + L"|" + client.computerName + L"|" + client.userName;
}