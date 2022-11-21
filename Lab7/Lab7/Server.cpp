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
#include <sstream>

#pragma comment(lib, "ws2_32.lib")

struct StateInfo
{
    HWND chatText = {};
    SOCKET socket = INVALID_SOCKET;
    fd_set master;
    bool isNull = true;
};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
StateInfo* GetAppState(HWND hwnd);
HWND CreateButton(HWND hwnd, LPCWSTR buttonText, int x, int y, int width, int height, int OnButtonClicked);
LRESULT Initialize(HWND hwnd);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"ServerClass";

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
        L"Server",
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

        if (pState != nullptr && !pState->isNull)
        {
            fd_set copy = pState->master;
            int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

            for (int i = 0; i < socketCount; i++)
            {
                SOCKET sock = copy.fd_array[i];

                if (sock == pState->socket)
                {
                    SOCKET client = accept(pState->socket, nullptr, nullptr);

                    FD_SET(client, &pState->master);

                    std::string welcomeMsg = "Connected to server.\n";
                    send(client, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
                }
                else
                {
                    char buffer[4096];
                    ZeroMemory(buffer, 4096);

                    int bytesIn = recv(sock, buffer, 4096, 0);

                    if (bytesIn <= 0)
                    {
                        closesocket(sock);
                        FD_CLR(sock, &pState->master);
                    }
                    else
                    {
                        for (int i = 0; i < pState->master.fd_count; i++)
                        {
                            SOCKET outSock = pState->master.fd_array[i];
                            if (outSock != pState->socket && outSock != sock)
                            {
                                std::ostringstream ss;
                                ss << "SOCKET #" << sock << ": " << buffer;
                                std::string strOut = ss.str();

                                send(outSock, strOut.c_str(), strOut.size() + 1, 0);
                            }
                        }
                    }
                }
            }
        }
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

    pState->chatText = CreateWindow(L"EDIT", L" ", WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_VSCROLL, 10, 10, 970, 440, hwnd, nullptr, nullptr, nullptr);

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // 1
    int iFamily = AF_INET;
    int iType = SOCK_STREAM;
    int iProtocol = IPPROTO_TCP;
    pState->socket = socket(iFamily, iType, iProtocol);

    // 2
    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr("127.0.0.1");
    service.sin_port = htons(27015);
    bind(pState->socket, (SOCKADDR*)&service, sizeof(service));

    // 3
    listen(pState->socket, SOMAXCONN);

    FD_ZERO(&pState->master);
    FD_SET(pState->socket, &pState->master);

    pState->isNull = false;

    return 0;
}
