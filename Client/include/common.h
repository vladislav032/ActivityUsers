#ifndef COMMON_H
#define COMMON_H

#include <windows.h>
#include <winsock2.h>

#if !defined(_WINSOCK2API_) && !defined(_WINSOCKAPI_)
struct timeval
{
    long tv_sec;
    long tv_usec;
};
#endif

extern SOCKET client_socket;
extern char computer_name[MAX_COMPUTERNAME_LENGTH + 1];
extern char user_name[256];
extern volatile BOOL bShouldExit;
extern volatile BOOL bConnected;

#endif