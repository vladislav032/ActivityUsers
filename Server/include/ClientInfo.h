#pragma once
#include "common.h"

struct ClientInfo
{
    SOCKET socket;
    std::wstring computerName;
    std::wstring userName;
    std::wstring ipAddress;
    time_t lastActivity;
    bool hasScreenshotRequest;
};