#include "ServerApp.h"

std::vector<ClientInfo> clients;
std::recursive_mutex clientsMutex;
std::atomic<bool> serverRunning(true);
HWND hListView = NULL;
HWND hMainWindow = NULL;

// обновление списка клиентов
void UpdateClientList()
{
    std::lock_guard<std::recursive_mutex> lock(clientsMutex);
    
    ListView_DeleteAllItems(hListView);

    std::vector<std::wstring> existingKeys;
    int count = ListView_GetItemCount(hListView);

    for (int i = 0; i < count; ++i)
    {
        wchar_t buffer[256];
        ListView_GetItemText(hListView, i, 0, buffer, 256);
        std::wstring ip = buffer;
        ListView_GetItemText(hListView, i, 1, buffer, 256);
        std::wstring comp = buffer;
        ListView_GetItemText(hListView, i, 2, buffer, 256);
        std::wstring user = buffer;
        existingKeys.push_back(ip + L"|" + comp + L"|" + user);
    }

    int index = 0;
    for (const auto &client : clients)
    {
        std::wstring key = GetClientKey(client);
        auto it = std::find(existingKeys.begin(), existingKeys.end(), key);

        wchar_t timeStr[20];
        wcsftime(timeStr, sizeof(timeStr) / sizeof(timeStr[0]), L"%H:%M:%S %d.%m.%Y", localtime(&client.lastActivity));

        if (it == existingKeys.end())
        {
            LVITEM lvItem = {0};
            lvItem.mask = LVIF_TEXT;
            lvItem.iItem = index;
            lvItem.iSubItem = 0;
            lvItem.pszText = (LPWSTR)client.ipAddress.c_str();
            ListView_InsertItem(hListView, &lvItem);

            ListView_SetItemText(hListView, index, 1, (LPWSTR)client.computerName.c_str());
            ListView_SetItemText(hListView, index, 2, (LPWSTR)client.userName.c_str());
            ListView_SetItemText(hListView, index, 3, timeStr);
        }
        else
        {
            int i = std::distance(existingKeys.begin(), it);
            ListView_SetItemText(hListView, i, 3, timeStr);
        }

        ++index;
    }
}

// отправка запроса на скриншот
void RequestScreenshot(int index)
{
    std::lock_guard<std::recursive_mutex> lock(clientsMutex);
    if (index >= 0 && index < (int)clients.size())
    {
        send(clients[index].socket, "GET_SCREENSHOT", 14, 0);
        MessageBox(hMainWindow, L"Запрос скриншота отправлен", L"Успешно", MB_OK | MB_ICONINFORMATION);
    }
}
// wndproc - функция callback для обработки msg, и работы с окном
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        hListView = CreateWindowW(L"SysListView32", L"",
                                  WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT | LVS_SINGLESEL,
                                  10, 10, 760, 400, hWnd, (HMENU)ID_LISTVIEW, NULL, NULL);

        LVCOLUMN lvc = {0};
        lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
        lvc.fmt = LVCFMT_LEFT;

        lvc.iSubItem = 0;
        lvc.pszText = (LPWSTR)L"IP-адрес";
        lvc.cx = 150;
        ListView_InsertColumn(hListView, 0, &lvc);

        lvc.iSubItem = 1;
        lvc.pszText = (LPWSTR)L"Имя компьютера";
        lvc.cx = 150;
        ListView_InsertColumn(hListView, 1, &lvc);

        lvc.iSubItem = 2;
        lvc.pszText = (LPWSTR)L"Пользователь";
        lvc.cx = 150;
        ListView_InsertColumn(hListView, 2, &lvc);

        lvc.iSubItem = 3;
        lvc.pszText = (LPWSTR)L"Последняя активность";
        lvc.cx = 250;
        ListView_InsertColumn(hListView, 3, &lvc);

        CreateWindowW(L"BUTTON", L"Обновить",
                      WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                      10, 420, 100, 30, hWnd, (HMENU)ID_REFRESH_BTN, NULL, NULL);

        CreateWindowW(L"BUTTON", L"Запросить скриншот",
                      WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                      120, 420, 150, 30, hWnd, (HMENU)ID_SCREENSHOT_BTN, NULL, NULL);

        CreateWindowW(L"BUTTON", L"Выход",
                      WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                      280, 420, 100, 30, hWnd, (HMENU)ID_QUIT_BTN, NULL, NULL);

        SetTimer(hWnd, ID_TIMER_UPDATE, 1000, NULL);
        break;
    }
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case ID_REFRESH_BTN:
            UpdateClientList();
            break;
        case ID_SCREENSHOT_BTN:
        {
            int selected = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);
            if (selected != -1)
            {
                RequestScreenshot(selected);
            }
            else
            {
                MessageBox(hWnd, L"Выберите клиента из списка", L"Ошибка", MB_OK | MB_ICONWARNING);
            }
            break;
        }
        case ID_QUIT_BTN:
            serverRunning = false;
            PostQuitMessage(0);
            break;
        }
        break;
    }
    case WM_UPDATE_LIST:
        UpdateClientList();
        break;
    case WM_DESTROY:
        KillTimer(hWnd, ID_TIMER_UPDATE);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
