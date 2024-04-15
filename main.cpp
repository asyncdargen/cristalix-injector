#include <windows.h>
#include <iostream>
#include <tlhelp32.h>

#include "process.h"
#include "main.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HWND stateLabel;
DWORD process;
std::string dllPath;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    ::ShowWindow(::GetConsoleWindow(), SW_HIDE);

    const char CLASS_NAME[] = "Sample Window Class";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, "Cristalix Mod Loader injector",
        (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX),
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 165,
        nullptr, nullptr, hInstance, nullptr
    );

    if (hwnd == nullptr) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    stateLabel = CreateWindowEx(
        0, "STATIC", "",
        WS_CHILD | WS_VISIBLE,
        50, 10, 300, 35,
        hwnd, nullptr, hInstance, nullptr);

    CreateWindow(
        "BUTTON", "Choose DLL",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        50, 55, 300, 30,
        hwnd, reinterpret_cast<HMENU>(1), hInstance, nullptr
    );


    CreateWindow(
        "BUTTON", "INJECT",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        50, 95, 300, 30,
        hwnd, reinterpret_cast<HMENU>(2), hInstance, nullptr
    );

    CreateThread(nullptr, 0, &RunProccessFindLoop, nullptr, 0, nullptr);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static std::string filePath;

    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case 1: {
                    OPENFILENAME ofn;
                    char szFile[MAX_PATH] = "";

                    ZeroMemory(&ofn, sizeof(ofn));
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = hwnd;
                    ofn.lpstrFile = szFile;
                    ofn.lpstrFile[0] = '\0';
                    ofn.nMaxFile = sizeof(szFile) / sizeof(*szFile);
                    ofn.lpstrFilter = "DLL Files (*.dll)\0*.dll\0";
                    ofn.nFilterIndex = 1;
                    ofn.lpstrFileTitle = nullptr;
                    ofn.nMaxFileTitle = 0;
                    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

                    if (GetOpenFileName(&ofn) == TRUE) {
                        filePath = ofn.lpstrFile;
                        dllPath = filePath;
                        UpdateStateLabel();
                    }
                }
                break;
                case 2:
                    if (!filePath.empty() && process != 0) {
                        InjectDLL(process, const_cast<char *>(filePath.c_str()));
                    }
            }
            break;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void UpdateStateLabel() {
    SetWindowText(stateLabel, (
                      "DLL: " + (dllPath.empty() ? "Not selected" : dllPath)
                      + "\nProcess: " + (process != 0 ? std::to_string(process) : "Not found")
                  ).c_str());
}
