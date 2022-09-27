#ifndef UNICODE
#define UNICODE
#endif 

#include <Windows.h>
#include <fstream>
#include <stdlib.h>

const int OnLeftButtonClicked = 1;
const int OnUpButtonClicked = 2;
const int OnDownButtonClicked = 3;
const int OnRightButtonClicked = 4;

struct StateInfo
{
    RECT workPlace = {};
    RECT* rectangle = new RECT;
    BOOLEAN isCaptured = FALSE;
    POINT point = {};
    HWND leftButton = {};
    HWND upButton = {};
    HWND downButton = {};
    HWND rightButton = {};
    HWND stepText = {};
    HWND stepEditText = {};
};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HWND CreateButton(HWND hwnd, LPCWSTR buttonText, int x, int y, int width, int height, int OnButtonClicked);
StateInfo* GetAppState(HWND hwnd);
LRESULT Initialize(HWND hwnd);
LRESULT OnSize(HWND hwnd);
LRESULT DefineWorkPlace(HWND hwnd);
LRESULT LoadWindow(HWND hwnd);
LRESULT LoadRectangle(HWND hwnd);
LRESULT SaveRectangle(HWND hwnd);
LRESULT SaveWindow(HWND hwnd);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"MainWindowClass";

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
        L"Square",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
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

    RegisterHotKey(hwnd, 1, MOD_CONTROL | MOD_NOREPEAT, 0x42);

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

    case WM_CLOSE:
    {
        SaveRectangle(hwnd);
        SaveWindow(hwnd);

        STARTUPINFO si;
        memset(&si, 0, sizeof(si));
        si.cb = sizeof(si);
        PROCESS_INFORMATION pi;
        CreateProcess(nullptr, ::GetCommandLine(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi);

        DestroyWindow(hwnd);
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_GETMINMAXINFO:
    {
        auto lpMMI = (LPMINMAXINFO)lParam;
        lpMMI->ptMinTrackSize.x = 345;
        lpMMI->ptMinTrackSize.y = 590;
        return 0;
    }

    case WM_HOTKEY:
        PostQuitMessage(0);
        return 0;

    case WM_COMMAND:
    {
        switch (wParam)
        {
        case OnLeftButtonClicked:
        {
            TCHAR stringStep[] = TEXT("STEP");
            GetWindowText(pState->stepEditText, stringStep, 5);
            int step = _wtoi(stringStep);
            pState->rectangle->left -= step;
            pState->rectangle->right -= step;

            if (pState->rectangle->left <= pState->workPlace.left)
            {
                pState->rectangle->left = pState->workPlace.left + 1;
                pState->rectangle->right = pState->workPlace.left + 51;
                InvalidateRect(hwnd, nullptr, FALSE);
                MessageBox(hwnd, L"You hit a wall!", L"No entry!", MB_OK);
            }

            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }

        case OnUpButtonClicked:
        {
            TCHAR stringStep[] = TEXT("STEP");
            GetWindowText(pState->stepEditText, stringStep, 5);
            int step = _wtoi(stringStep);
            pState->rectangle->top -= step;
            pState->rectangle->bottom -= step;

            if (pState->rectangle->top <= pState->workPlace.top)
            {
                pState->rectangle->top = pState->workPlace.top + 1;
                pState->rectangle->bottom = pState->workPlace.top + 51;
                InvalidateRect(hwnd, nullptr, FALSE);
                MessageBox(hwnd, L"You hit a wall!", L"No entry!", MB_OK);
            }

            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }

        case OnDownButtonClicked:
        {
            TCHAR stringStep[] = TEXT("STEP");
            GetWindowText(pState->stepEditText, stringStep, 5);
            int step = _wtoi(stringStep);
            pState->rectangle->top += step;
            pState->rectangle->bottom += step;

            if (pState->rectangle->bottom >= pState->workPlace.bottom)
            {
                pState->rectangle->top = pState->workPlace.bottom - 51;
                pState->rectangle->bottom = pState->workPlace.bottom - 1;
                InvalidateRect(hwnd, nullptr, FALSE);
                MessageBox(hwnd, L"You hit a wall!", L"No entry!", MB_OK);
            }

            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }

        case OnRightButtonClicked:
        {
            TCHAR stringStep[] = TEXT("STEP");
            GetWindowText(pState->stepEditText, stringStep, 5);
            int step = _wtoi(stringStep);
            pState->rectangle->left += step;
            pState->rectangle->right += step;

            if (pState->rectangle->right >= pState->workPlace.right)
            {
                pState->rectangle->left = pState->workPlace.right - 51;
                pState->rectangle->right = pState->workPlace.right - 1;
                InvalidateRect(hwnd, nullptr, FALSE);
                MessageBox(hwnd, L"You hit a wall!", L"No entry!", MB_OK);
            }

            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }

        default:
            return 0;
        }
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        Rectangle(hdc, pState->workPlace.left, pState->workPlace.top, pState->workPlace.right, pState->workPlace.bottom);
        FillRect(hdc, pState->rectangle, (HBRUSH)(COLOR_WINDOW + 2));
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_SIZE:
    {
        OnSize(hwnd);
        return 0;
    }

    case WM_LBUTTONDOWN:
    {
        SetCapture(hwnd);
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);

        if (x >= pState->rectangle->left && x <= pState->rectangle->right && y >= pState->rectangle->top && y <= pState->rectangle->bottom)
        {
            pState->point.x = x;
            pState->point.y = y;
            pState->isCaptured = TRUE;
        }
        return 0;
    }

    case WM_LBUTTONUP:
    {
        pState->isCaptured = FALSE;
        ReleaseCapture();
        return 0;
    }

    case WM_MOUSEMOVE:
    {
        if ((DWORD)wParam & MK_LBUTTON && pState->isCaptured)
        {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);

            if (x > pState->workPlace.left && x < pState->workPlace.right && y > pState->workPlace.top && y < pState->workPlace.bottom)
            {
                int left = pState->rectangle->left;
                int top = pState->rectangle->top;
                int right = pState->rectangle->right;
                int bottom = pState->rectangle->bottom;
                pState->rectangle->left = pState->rectangle->left + (x - pState->point.x);
                pState->rectangle->top = pState->rectangle->top + (y - pState->point.y);
                pState->rectangle->right = pState->rectangle->right + (x - pState->point.x);
                pState->rectangle->bottom = pState->rectangle->bottom + (y - pState->point.y);

                if (pState->workPlace.right - pState->rectangle->right > 0 && pState->workPlace.bottom - pState->rectangle->bottom > 0 &&
                    pState->rectangle->left - pState->workPlace.left > 0 && pState->rectangle->top - pState->workPlace.top > 0)
                {
                    InvalidateRect(hwnd, nullptr, FALSE);
                    pState->point.x = x;
                    pState->point.y = y;
                }
                else
                {
                    pState->rectangle->left = left;
                    pState->rectangle->top = top;
                    pState->rectangle->right = right;
                    pState->rectangle->bottom = bottom;
                }
            }
        }
        return 0;
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
    DefineWorkPlace(hwnd);

    auto pState = GetAppState(hwnd);

    pState->rectangle->left = pState->workPlace.left + 1;
    pState->rectangle->top = pState->workPlace.top + 1;
    pState->rectangle->right = pState->workPlace.left + 51;
    pState->rectangle->bottom = pState->workPlace.top + 51;

    pState->leftButton = CreateButton(hwnd, L"<", pState->workPlace.left, pState->workPlace.top / 4, 50, pState->workPlace.top * 2 / 4, OnLeftButtonClicked);
    pState->upButton = CreateButton(hwnd, L"/\\", pState->workPlace.left + 50, pState->workPlace.top / 4, 50, pState->workPlace.top * 2 / 4, OnUpButtonClicked);
    pState->downButton = CreateButton(hwnd, L"\\/", pState->workPlace.left + 100, pState->workPlace.top / 4, 50, pState->workPlace.top * 2 / 4, OnDownButtonClicked);
    pState->rightButton = CreateButton(hwnd, L">", pState->workPlace.left + 150, pState->workPlace.top / 4, 50, pState->workPlace.top * 2 / 4, OnRightButtonClicked);
    pState->stepText = CreateWindow(L"STATIC", L"STEP", WS_VISIBLE | WS_CHILD, pState->workPlace.left + 210, pState->workPlace.top / 4, 35, pState->workPlace.top / 4, hwnd, nullptr, nullptr, nullptr);
    pState->stepEditText = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD, pState->workPlace.left + 210, pState->workPlace.top * 2 / 4, 35, pState->workPlace.top / 4, hwnd, nullptr, nullptr, nullptr);

    LoadWindow(hwnd);
    return 0;
}

LRESULT OnSize(HWND hwnd)
{
    DefineWorkPlace(hwnd);

    auto pState = GetAppState(hwnd);

    LoadRectangle(hwnd);

    if (pState->rectangle->left <= pState->workPlace.left)
    {
        pState->rectangle->left = pState->workPlace.left + 1;
        pState->rectangle->right = pState->workPlace.left + 51;
        InvalidateRect(hwnd, nullptr, FALSE);
    }

    if (pState->rectangle->top <= pState->workPlace.top)
    {
        pState->rectangle->top = pState->workPlace.top + 1;
        pState->rectangle->bottom = pState->workPlace.top + 51;
        InvalidateRect(hwnd, nullptr, FALSE);
    }

    if (pState->rectangle->right >= pState->workPlace.right)
    {
        pState->rectangle->left = pState->workPlace.right - 51;
        pState->rectangle->right = pState->workPlace.right - 1;
        InvalidateRect(hwnd, nullptr, FALSE);
    }

    if (pState->rectangle->bottom >= pState->workPlace.bottom)
    {
        pState->rectangle->top = pState->workPlace.bottom - 51;
        pState->rectangle->bottom = pState->workPlace.bottom - 1;
        InvalidateRect(hwnd, nullptr, FALSE);
    }

    MoveWindow(pState->leftButton, pState->workPlace.left, pState->workPlace.top / 4, 50, pState->workPlace.top * 2 / 4, TRUE);
    MoveWindow(pState->upButton, pState->workPlace.left + 50, pState->workPlace.top / 4, 50, pState->workPlace.top * 2 / 4, TRUE);
    MoveWindow(pState->downButton, pState->workPlace.left + 100, pState->workPlace.top / 4, 50, pState->workPlace.top * 2 / 4, TRUE);
    MoveWindow(pState->rightButton, pState->workPlace.left + 150, pState->workPlace.top / 4, 50, pState->workPlace.top * 2 / 4, TRUE);
    MoveWindow(pState->stepText, pState->workPlace.left + 210, pState->workPlace.top / 4, 35, pState->workPlace.top / 4, TRUE);
    MoveWindow(pState->stepEditText, pState->workPlace.left + 210, pState->workPlace.top * 2 / 4, 35, pState->workPlace.top / 4, TRUE);
    InvalidateRect(hwnd, nullptr, TRUE);
    return 0;
}

LRESULT DefineWorkPlace(HWND hwnd)
{
    StateInfo* pState = GetAppState(hwnd);
    GetClientRect(hwnd, &pState->workPlace);
    pState->workPlace.left = (pState->workPlace.right - pState->workPlace.left) / 8;
    pState->workPlace.top = (pState->workPlace.bottom - pState->workPlace.top) / 8;
    pState->workPlace.right = pState->workPlace.right * 7 / 8;
    pState->workPlace.bottom = pState->workPlace.bottom * 7 / 8;
    return 0;
}

LRESULT LoadWindow(HWND hwnd)
{
    RECT window;
    GetWindowRect(hwnd, &window);

    char stringStep[5] = "";

    std::ifstream file1("window.left.dat");
    std::ifstream file2("window.top.dat");
    std::ifstream file3("window.bottom.dat");
    std::ifstream file4("window.right.dat");
    std::ifstream file5("window.step.dat");

    if (file1.good() == true)
    {
        file1 >> window.left;
        file1.close();
        file2 >> window.top;
        file2.close();
        file3 >> window.bottom;
        file3.close();
        file4 >> window.right;
        file4.close();
        file5 >> stringStep;
        file5.close();
        remove("window.left.dat");
        remove("window.top.dat");
        remove("window.bottom.dat");
        remove("window.right.dat");
        remove("window.step.dat");
    }

    auto pState = GetAppState(hwnd);
    TCHAR step[5] = TEXT("");
    mbstowcs_s(nullptr, step, stringStep, strlen(stringStep));
    SetWindowTextW(pState->stepEditText, step);

    MoveWindow(hwnd, window.left, window.top, window.right - window.left, window.bottom - window.top, TRUE);
    return 0;
}

LRESULT LoadRectangle(HWND hwnd)
{
    std::ifstream file1("rectangle.left.dat");
    std::ifstream file2("rectangle.top.dat");
    std::ifstream file3("rectangle.bottom.dat");
    std::ifstream file4("rectangle.right.dat");

    auto pState = GetAppState(hwnd);

    if (file1.good() == true)
    {
        file1 >> pState->rectangle->left;
        file1.close();
        file2 >> pState->rectangle->top;
        file2.close();
        file3 >> pState->rectangle->bottom;
        file3.close();
        file4 >> pState->rectangle->right;
        file4.close();
        remove("rectangle.left.dat");
        remove("rectangle.top.dat");
        remove("rectangle.bottom.dat");
        remove("rectangle.right.dat");
    }

    return 0;
}

LRESULT SaveRectangle(HWND hwnd)
{
    auto pState = GetAppState(hwnd);

    std::ofstream file1("rectangle.left.dat");
    file1 << (int)pState->rectangle->left << std::endl;
    file1.close();
    std::ofstream file2("rectangle.top.dat");
    file2 << (int)pState->rectangle->top << std::endl;
    file2.close();
    std::ofstream file3("rectangle.bottom.dat");
    file3 << (int)pState->rectangle->bottom << std::endl;
    file3.close();
    std::ofstream file4("rectangle.right.dat");
    file4 << (int)pState->rectangle->right << std::endl;
    file4.close();
    return 0;
}

LRESULT SaveWindow(HWND hwnd)
{
    RECT window;
    GetWindowRect(hwnd, &window);

    std::ofstream file1("window.left.dat");
    file1 << (int)window.left << std::endl;
    file1.close();
    std::ofstream file2("window.top.dat");
    file2 << (int)window.top << std::endl;
    file2.close();
    std::ofstream file3("window.bottom.dat");
    file3 << (int)window.bottom << std::endl;
    file3.close();
    std::ofstream file4("window.right.dat");
    file4 << (int)window.right << std::endl;
    file4.close();

    auto pState = GetAppState(hwnd);
    TCHAR stringStep[] = TEXT("STEP");
    GetWindowText(pState->stepEditText, stringStep, 5);
    char step[5];
    wcstombs_s(nullptr, step, stringStep, wcslen(stringStep) + 1);

    std::ofstream file5("window.step.dat");
    file5 << step << std::endl;
    file5.close();
    return 0;
}
