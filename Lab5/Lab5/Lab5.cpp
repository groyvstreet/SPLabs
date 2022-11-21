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

typedef struct structDATATOSERVER
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
    PCOPYDATASTRUCT pCDS;
    std::vector<DATA> messages;
    DATATOCLIENT data;
};

COPYDATASTRUCT CDS;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
StateInfo* GetAppState(HWND hwnd);
HWND CreateButton(HWND hwnd, LPCWSTR buttonText, int x, int y, int width, int height, int OnButtonClicked);
LRESULT Initialize(HWND hwnd);
BOOL CALLBACK EnumWndProc(HWND wnd, LPARAM lParam);

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
        DATA data = {};

        for (int i = 0; i < 100; i++)
        {
            data.username[i] = ((DATA*)pState->pCDS->lpData)->username[i];
        }

        for (int i = 0; i < 1000; i++)
        {
            data.message[i] = ((DATA*)pState->pCDS->lpData)->message[i];
        }

        pState->messages.push_back(data);

        int size = pState->messages.size() * 1109;

        TCHAR* chat = new TCHAR[size + 1];
        int k = 0;

        for (int i = 0; i < pState->messages.size(); i++)
        {
            for (int j = 0; j < 100; j++)
            {
                if (pState->messages[i].username[j] == '\0')
                {
                    break;
                }

                chat[k] = pState->messages[i].username[j];
                k++;
            }

            chat[k] = ':';
            k++;
            chat[k] = '\r';
            k++;
            chat[k] = '\n';
            k++;
            chat[k] = '\t';
            k++;

            for (int j = 0; j < 1000; j++)
            {
                if (pState->messages[i].message[j] == '\0')
                {
                    break;
                }

                chat[k] = pState->messages[i].message[j];
                k++;
            }

            chat[k] = '\r';
            k++;
            chat[k] = '\n';
            k++;
            chat[k] = '\r';
            k++;
            chat[k] = '\n';
            k++;
        }

        chat[k] = '\0';
        SetWindowText(pState->chatText, chat);

        HWND hwndClient = FindWindow(NULL, L"Client");

        for (int i = 0; i < size; i++)
        {
            pState->data.chat[i] = chat[i];

            if (chat[i] == '\0')
            {
                break;
            }
        }

        CDS.dwData = 1;
        CDS.cbData = sizeof(pState->data);
        CDS.lpData = &pState->data;

        EnumWindows(EnumWndProc, (LPARAM)hwnd);
        return 0;
    }

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

    return 0;
}

BOOL CALLBACK EnumWndProc(HWND hwnd, LPARAM lParam) {
    if (hwnd == (HWND)lParam) return TRUE;

    TCHAR class_name[256];
    GetClassName(hwnd, class_name, 256);

    if (wcscmp(class_name, L"ClientClass") == 0) {
        SendMessage(hwnd, WM_COPYDATA, NULL, (LPARAM)(LPVOID)&CDS);
    }

    return TRUE;
}
