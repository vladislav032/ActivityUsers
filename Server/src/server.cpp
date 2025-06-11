#include "ServerApp.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"ServerAppClass";

    if (!RegisterClassExW(&wc))
    {
        MessageBox(NULL, L"Ошибка регистрации класса окна", L"Ошибка", MB_OK | MB_ICONERROR);
        return 1;
    }

    hMainWindow = CreateWindowW(L"ServerAppClass", L"Сервер мониторинга клиентов",
                                WS_OVERLAPPEDWINDOW,
                                CW_USEDEFAULT, CW_USEDEFAULT, 800, 500,
                                NULL, NULL, hInstance, NULL);

    if (!hMainWindow)
    {
        MessageBox(NULL, L"Ошибка создания окна", L"Ошибка", MB_OK | MB_ICONERROR);
        return 1;
    }

    ShowWindow(hMainWindow, nCmdShow);
    UpdateWindow(hMainWindow);

    CreateThread(NULL, 0, ServerThread, NULL, 0, NULL);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}