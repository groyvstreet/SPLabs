#ifndef UNICODE
#define UNICODE
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

const int OnSendMessageButtonClicked = 1;

typedef struct structDATA
{
    TCHAR username[100];
    TCHAR message[1000];
} DATA;

typedef struct structDATATOCLIENT
{
    TCHAR chat[100000];
} DATATOCLIENT;

struct StateInfo
{
    HWND chatText = {};
    HWND usernameEdit = {};
    HWND messageEdit = {};
    HWND sendButton = {};
    COPYDATASTRUCT CDS;
    PCOPYDATASTRUCT pCDS;
    DATA data;
};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
StateInfo* GetAppState(HWND hwnd);
HWND CreateButton(HWND hwnd, LPCWSTR buttonText, int x, int y, int width, int height, int OnButtonClicked);
LRESULT Initialize(HWND hwnd);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"ClientClass";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    RegisterClass(&wc);

    auto pState = new StateInfo;
    if (pState == nullptr)
    {
        return 0;
    }

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Client",
        WS_OVERLAPPED | WS_SYSMENU,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        1000,
        500,
        nullptr,
        nullptr,
        hInstance,
        pState
    );

    if (hwnd == nullptr)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = { };
    while (GetMessage(&msg, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    StateInfo* pState;
    if (uMsg == WM_CREATE)
    {
        auto pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        pState = reinterpret_cast<StateInfo*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pState);
    }
    else
    {
        pState = GetAppState(hwnd);
    }

    switch (uMsg)
    {
    case WM_CREATE:
        Initialize(hwnd);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_COPYDATA:
    {
        pState->pCDS = (PCOPYDATASTRUCT)lParam;
        DATATOCLIENT dataToClient = {};

        for (int i = 0; i < 100000; i++)
        {
            dataToClient.chat[i] = ((DATATOCLIENT*)pState->pCDS->lpData)->chat[i];

            if (((DATATOCLIENT*)pState->pCDS->lpData)->chat[i] == '\0')
            {
                break;
            }
        }

        SetWindowText(pState->chatText, dataToClient.chat);
        return 0;
    }

    case WM_COMMAND:
    {
        switch (wParam)
        {
        case OnSendMessageButtonClicked:
        {
            HWND hwndServer = FindWindow(NULL, L"Server");
            WIN32_FIND_DATA FindFileData;
            HANDLE hFile;

            if (!hwndServer)
            {
                hFile = FindFirstFile(L"Lab5.exe", &FindFileData);
                WinExec("Lab5.exe", 1);
                FindClose(hFile);
            }

            TCHAR username[] = TEXT("___________________________________________________________________________________________________");
            GetWindowText(pState->usernameEdit, username, 100);

            for (int i = 0; i < 100; i++)
            {
                pState->data.username[i] = username[i];
            }

            TCHAR message[] = TEXT("_______________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________");
            GetWindowText(pState->messageEdit, message, 1000);

            for (int i = 0; i < 1000; i++)
            {
                pState->data.message[i] = message[i];
            }

            pState->CDS.dwData = 1;
            pState->CDS.cbData = sizeof(pState->data);
            pState->CDS.lpData = &pState->data;

            SendMessage(hwndServer, WM_COPYDATA, NULL, (LPARAM)(LPVOID)&pState->CDS);
            return 0;
        }

        default:
            return 0;
        }
    }

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

HWND CreateButton(HWND hwnd, LPCWSTR buttonText, int x, int y, int width, int height, int OnButtonClicked)
{
    return CreateWindow(
        L"BUTTON",
        buttonText,
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        x,
        y,
        width,
        height,
        hwnd,
        (HMENU)OnButtonClicked,
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        nullptr
    );
}

StateInfo* GetAppState(HWND hwnd)
{
    LONG_PTR ptr = GetWindowLongPtr(hwnd, GWLP_USERDATA);
    auto pState = reinterpret_cast<StateInfo*>(ptr);
    return pState;
}

LRESULT Initialize(HWND hwnd)
{
    auto pState = GetAppState(hwnd);

    pState->chatText = CreateWindow(L"EDIT", L" ", WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_VSCROLL, 10, 10, 970, 300, hwnd, nullptr, nullptr, nullptr);
    pState->usernameEdit = CreateWindow(L"EDIT", L" ", WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_VSCROLL, 10, 320, 970, 20, hwnd, nullptr, nullptr, nullptr);
    pState->messageEdit = CreateWindow(L"EDIT", L" ", WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_VSCROLL, 10, 350, 970, 40, hwnd, nullptr, nullptr, nullptr);
    pState->sendButton = CreateButton(hwnd, L"Send", 450, 400, 100, 20, OnSendMessageButtonClicked);

    return 0;
}
