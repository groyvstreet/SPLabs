#ifndef UNICODE
#define UNICODE
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

const int OnSendMessageButtonClicked = 1;

struct StateInfo
{
    HWND chatText = {};
    HWND messageEdit = {};
    HWND sendButton = {};
    SOCKET socket = INVALID_SOCKET;
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

        char buffer[4096] = "";

        int iResult = recv(pState->socket, buffer, 4096, MSG_PEEK);

        wchar_t wtext[4096];
        mbstowcs(wtext, buffer, 4096);

        SetWindowText(pState->chatText, wtext);
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

    case WM_COMMAND:
    {
        switch (wParam)
        {

        case OnSendMessageButtonClicked:
        {
            wchar_t message[4096];
            GetWindowText(pState->messageEdit, message, 4096);
            char sendbuf[4096] = "";

            int i;
            for (i = 0; i < 4094; i++)
            {
                sendbuf[i] = message[i];

                if (message[i] == '\0')
                {
                    break;
                }
            }

            sendbuf[i] = '\r';
            sendbuf[++i] = '\n';

            send(pState->socket, sendbuf, (int)strlen(sendbuf), 0);
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
    pState->messageEdit = CreateWindow(L"EDIT", L" ", WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_VSCROLL, 10, 350, 970, 40, hwnd, nullptr, nullptr, nullptr);
    pState->sendButton = CreateButton(hwnd, L"Send", 450, 400, 100, 20, OnSendMessageButtonClicked);

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // 1
    int iFamily = AF_INET;
    int iType = SOCK_STREAM;
    int iProtocol = IPPROTO_TCP;
    pState->socket = socket(iFamily, iType, iProtocol);

    // 2
    struct sockaddr_in clientService;

    int recvbuflen = 512;
    char sendbuf[] = "Client: sending data test";
    char recvbuf[512] = "";

    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr("127.0.0.1");
    clientService.sin_port = htons(27015);

    connect(pState->socket, (SOCKADDR*)&clientService, sizeof(clientService));

    return 0;
}
