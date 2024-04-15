#include <algorithm>
#include <iostream>
#include "main.h"

#include "process.h"
#include <tlhelp32.h>
#include <psapi.h>
#include <windows.h>

using namespace std;

DWORD WINAPI RunProccessFindLoop(LPVOID) {
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    TCHAR filename[MAX_PATH];
    HANDLE snapshot;
    bool found;

    while (true) {
        snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        found = false;

        if (Process32First(snapshot, &entry) == TRUE) {
            while (Process32Next(snapshot, &entry) == TRUE) {
                if (stricmp(entry.szExeFile, "java.exe") == 0) {
                    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
                    if (hProcess == nullptr) continue;

                    if (GetModuleFileNameEx(hProcess, nullptr, filename, MAX_PATH) != 0
                        && strstr(filename, ".cristalix")) {
                        // cout << entry.szExeFile << " : " << entry.th32ProcessID << ": " << filename << endl;
                        ::process = entry.th32ProcessID;
                        UpdateStateLabel();
                        found = true;
                        break;
                    }
                    CloseHandle(hProcess);
                }
            }
        }

        if (!found) {
            process = 0;
            UpdateStateLabel();
        }

        CloseHandle(snapshot);
        Sleep(500);
    }

    return 0;
}

void InjectDLL(DWORD process, char *dll) {
    cout << "Inject to " << process << " with dll " << dll << endl;
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process);
    if (hProcess == nullptr) {
        return;
    }

    LPVOID pDllPath = VirtualAllocEx(hProcess, nullptr, strlen(dll) + 1,
                                     MEM_COMMIT, PAGE_READWRITE);

    WriteProcessMemory(hProcess, pDllPath, dll, strlen(dll) + 1, 0);

    HANDLE hLoadThread = CreateRemoteThread(hProcess, nullptr, 0,
                                            reinterpret_cast<LPTHREAD_START_ROUTINE>(GetProcAddress(
                                                GetModuleHandleA("Kernel32.dll"),
                                                "LoadLibraryA")), pDllPath, 0, nullptr);

    WaitForSingleObject(hLoadThread, INFINITE);

    VirtualFreeEx(hProcess, pDllPath, strlen(dll) + 1, MEM_RELEASE);
    CloseHandle(hProcess);
}
