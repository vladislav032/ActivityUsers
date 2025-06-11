#ifndef COMMANDS_H
#define COMMANDS_H

#include "common.h"

void HandleServerCommands();
DWORD WINAPI CommandHandlerThread(LPVOID lpParam);

#endif