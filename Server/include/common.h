#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define UNICODE
#define _UNICODE

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <ctime>
#include <thread>
#include <mutex>
#include <atomic>
#include <commctrl.h>
#include <algorithm>
#include <cwchar>
#include <cstring>
#include <chrono>

// константы
constexpr int SERVER_PORT = 12345;
constexpr int BUFFER_SIZE = 1024;
constexpr int SCREENSHOT_BUFFER_SIZE = (1024 * 1024 * 5);

// идентификаторы элементов UI
constexpr int ID_LISTVIEW = 1001;
constexpr int ID_REFRESH_BTN = 1002;
constexpr int ID_SCREENSHOT_BTN = 1003;
constexpr int ID_QUIT_BTN = 1004;
constexpr int ID_TIMER_UPDATE = 1005;
constexpr int WM_UPDATE_LIST = (WM_USER + 1);