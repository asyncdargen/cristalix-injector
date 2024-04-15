#ifndef PROCESSFINDER_H
#define PROCESSFINDER_H

#include <windows.h>

DWORD WINAPI RunProccessFindLoop(LPVOID);

void InjectDLL(DWORD, char*);

#endif //PROCESSFINDER_H
