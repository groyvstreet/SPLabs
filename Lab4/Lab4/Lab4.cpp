#ifndef UNICODE
#define UNICODE
#endif 

#include <Windows.h>
#include <string>
#include <random>
#include <vector>
#include <thread>
#include <chrono>

const int OnWriteFileButtonClicked = 1;
const int OnReadFileButtonClicked = 2;
const int OnWriteFileAsyncButtonClicked = 3;
const int OnReadFileAsyncButtonClicked = 4;

struct StateInfo
{
    HWND arrayInputEdit = {};
    HWND arrayOutputEdit = {};
    HWND writeFileButton = {};
    HWND readFileButton = {};
    HWND writeFileAsyncButton = {};
    HWND readFileAsyncButton = {};
    HWND timeText1 = {};
    HWND timeText2 = {};
    HWND arraySizeText = {};
    HWND arraySizeInput = {};
    HWND threadsAmountText = {};
    HWND threadsAmountInput = {};
    HWND printText = {};
    HWND printEdit = {};
};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
StateInfo* GetAppState(HWND hwnd);
HWND CreateButton(HWND hwnd, LPCWSTR buttonText, int x, int y, int width, int height, int OnButtonClicked);
LRESULT Initialize(HWND hwnd);
void FillArray(int* array, int size);
void MergeSort(int* array, int left, int right);
void Merge(int* array, int left, int mid, int right);

struct Arguments
{
    int* array;
    int left;
    int right;
};

int threadsAmount = 0;

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
        L"Lab4",
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

    case WM_COMMAND:
    {
        TCHAR stringThreadsAmount[] = TEXT("__");
        GetWindowText(pState->threadsAmountInput, stringThreadsAmount, 3);
        threadsAmount = _wtoi(stringThreadsAmount);

        TCHAR stringToPrint[] = TEXT("_");
        GetWindowText(pState->printEdit, stringToPrint, 2);
        int toPrint = _wtoi(stringToPrint);

        switch (wParam)
        {
        case OnWriteFileButtonClicked:
        {
            TCHAR stringArraySize[] = TEXT("__________");
            GetWindowText(pState->arraySizeInput, stringArraySize, 11);
            int arraySize = 512 * _wtoi(stringArraySize);

            int* array = new int[arraySize];
            FillArray(array, arraySize);

            std::string input = "";

            if (toPrint == 1)
            {
                for (int i = 0; i < arraySize; i++)
                {
                    input += std::to_string(array[i]);
                    input += " ";
                }

                int size_w = input.length();
                TCHAR* input_w = new TCHAR[size_w + 1];
                input_w[size_w] = '\0';

                copy(input.begin(), input.end(), input_w);
                SetWindowText(pState->arrayInputEdit, input_w);
            }

            DWORD dwDone, dwStart, dwEnd;
            HANDLE hFile;

            hFile = CreateFile(L"file.dat", GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS, 0, nullptr);

            dwStart = GetTickCount64();
            WriteFile(hFile, array, arraySize * sizeof(int), &dwDone, nullptr);
            dwEnd = GetTickCount64();

            CloseHandle(hFile);

            input = std::to_string(dwEnd - dwStart);
            TCHAR* time = new TCHAR[100];
            time[input.length()] = '\0';
            copy(input.begin(), input.end(), time);
            SetWindowText(pState->timeText2, time);

            delete[] array;

            return 0;
        }

        case OnReadFileButtonClicked:
        {
            TCHAR stringArraySize[] = TEXT("__________");
            GetWindowText(pState->arraySizeInput, stringArraySize, 11);
            int arraySize = 512 * _wtoi(stringArraySize);

            int* array = new int[arraySize];

            DWORD dwDone, dwStart, dwEnd;
            HANDLE hFile;

            hFile = CreateFile(L"file.dat", GENERIC_READ, 0, nullptr, OPEN_ALWAYS, 0, nullptr);

            dwStart = GetTickCount();
            ReadFile(hFile, array, arraySize * sizeof(int), &dwDone, nullptr);
            MergeSort(array, 0, arraySize - 1);
            dwEnd = GetTickCount();

            CloseHandle(hFile);

            std::string input = std::to_string(dwEnd - dwStart);
            TCHAR* time = new TCHAR[100];
            time[input.length()] = '\0';
            copy(input.begin(), input.end(), time);
            SetWindowText(pState->timeText2, time);

            if (toPrint == 1)
            {
                input = "";

                for (int i = 0; i < arraySize; i++)
                {
                    input += std::to_string(array[i]);
                    input += " ";
                }

                int size_w = input.length();
                TCHAR* input_w = new TCHAR[size_w + 1];
                input_w[size_w] = '\0';

                copy(input.begin(), input.end(), input_w);
                SetWindowText(pState->arrayOutputEdit, input_w);
            }

            delete[] array;

            return 0;
        }

        case OnWriteFileAsyncButtonClicked:
        {
            TCHAR stringArraySize[] = TEXT("__________");
            GetWindowText(pState->arraySizeInput, stringArraySize, 11);
            int arraySize = 512 * _wtoi(stringArraySize);

            int* array = new int[arraySize];
            FillArray(array, arraySize);

            DWORD dwDone, dwStart, dwEnd;
            HANDLE hFile;

            hFile = CreateFile(L"file.dat", GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS, FILE_FLAG_OVERLAPPED |
                FILE_FLAG_NO_BUFFERING, nullptr);

            std::vector<OVERLAPPED> overlappeds;

            dwStart = GetTickCount();

            for (auto i = 0; i < threadsAmount + 1; i++)
            {
                OVERLAPPED overlapped = {};
                overlapped.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
                overlapped.Offset += arraySize * sizeof(int) * i / (threadsAmount + 1);

                WriteFile(hFile, array + i * arraySize / (threadsAmount + 1), arraySize * sizeof(int) / (threadsAmount + 1),
                    &dwDone, &overlapped);

                overlappeds.push_back(overlapped);
            }

            for (auto o : overlappeds)
            {
                WaitForSingleObject(o.hEvent, INFINITE);
                CloseHandle(o.hEvent);
            }

            dwEnd = GetTickCount();

            CloseHandle(hFile);

            std::string input = std::to_string(dwEnd - dwStart);
            TCHAR* time = new TCHAR[100];
            time[input.length()] = '\0';
            copy(input.begin(), input.end(), time);
            SetWindowText(pState->timeText2, time);

            input = "";

            if (toPrint == 1)
            {
                for (int i = 0; i < arraySize; i++)
                {
                    input += std::to_string(array[i]);
                    input += " ";
                }

                int size_w = input.length();
                TCHAR* input_w = new TCHAR[size_w + 1];
                input_w[size_w] = '\0';

                copy(input.begin(), input.end(), input_w);
                SetWindowText(pState->arrayInputEdit, input_w);
            }

            delete[] array;

            return 0;
        }

        case OnReadFileAsyncButtonClicked:
        {
            TCHAR stringArraySize[] = TEXT("__________");
            GetWindowText(pState->arraySizeInput, stringArraySize, 11);
            int arraySize = 512 * _wtoi(stringArraySize);

            int* array = new int[arraySize];

            DWORD dwDone, dwStart, dwEnd;
            HANDLE hFile;

            hFile = CreateFile(L"file.dat", GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_ALWAYS, FILE_FLAG_OVERLAPPED |
                FILE_FLAG_NO_BUFFERING, nullptr);

            std::vector<OVERLAPPED> overlappeds;
            int offset = 0;
            dwStart = GetTickCount();

            for (auto i = 0; i < threadsAmount + 1; i++)
            {
                OVERLAPPED overlapped = {};
                overlapped.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
                overlapped.Offset += arraySize * sizeof(int) * offset / (threadsAmount + 1);
                offset++;

                ReadFile(hFile, array + i * arraySize / (threadsAmount + 1), arraySize * sizeof(int) / (threadsAmount + 1),
                    &dwDone, &overlapped);

                overlappeds.push_back(overlapped);
            }

            /*OVERLAPPED overlapped = {};
            overlapped.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
            overlapped.Offset += arraySize * sizeof(int) * offset / (threadsAmount + 1);
            ReadFile(hFile, array + arraySize * offset / (threadsAmount + 1), arraySize * sizeof(int) / (threadsAmount + 1), &dwDone, &overlapped);
            WaitForSingleObject(overlapped.hEvent, INFINITE);
            MergeSort(array, overlapped.Offset / sizeof(int), overlapped.Offset / sizeof(int) + arraySize / (threadsAmount + 1) - 1);*/

            std::vector<std::thread> threads;

            for (auto o : overlappeds)
            {
                WaitForSingleObject(o.hEvent, INFINITE);
                CloseHandle(o.hEvent);
                std::thread thread(MergeSort, array, o.Offset / sizeof(int), o.Offset / sizeof(int) + arraySize / (threadsAmount + 1) - 1);
                threads.push_back(move(thread));
                //MergeSort(array, o.Offset / sizeof(int), o.Offset / sizeof(int) + arraySize / (threadsAmount + 1) - 1);
            }

            for (auto i = 0; i < threads.size(); i++)
            {
                threads[i].join();
            }

            auto i = 0;
            threadsAmount++;

            while (threadsAmount > 1)
            {
                int left = i * arraySize / threadsAmount;
                int right = left + 2 * arraySize / threadsAmount - 1;
                int mid = (left + right) / 2;
                Merge(array, left, mid, right);
                i += 2;

                if (i >= threadsAmount)
                {
                    threadsAmount /= 2;
                    i = 0;
                }
            }

            dwEnd = GetTickCount();

            /*OVERLAPPED overlapped1 = {};
            OVERLAPPED overlapped2 = {};
            overlapped1.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
            overlapped2.Offset += arraySize * sizeof(int) / 2;

            dwStart = GetTickCount();
            ReadFile(hFile, array, arraySize * sizeof(int) / 2, &dwDone, &overlapped1);
            ReadFile(hFile, array + arraySize / 2, arraySize * sizeof(int) / 2, &dwDone, &overlapped2);
            WaitForSingleObject(overlapped1.hEvent, INFINITE);
            dwEnd = GetTickCount();

            CloseHandle(overlapped1.hEvent);
            CloseHandle(overlapped2.hEvent);*/

            CloseHandle(hFile);

            std::string input = std::to_string(dwEnd - dwStart);
            TCHAR* time = new TCHAR[100];
            time[input.length()] = '\0';
            copy(input.begin(), input.end(), time);
            SetWindowText(pState->timeText2, time);

            if (toPrint == 1)
            {
                input = "";

                for (int i = 0; i < arraySize; i++)
                {
                    input += std::to_string(array[i]);
                    input += " ";
                }

                int size_w = input.length();
                TCHAR* input_w = new TCHAR[size_w + 1];
                input_w[size_w] = '\0';

                copy(input.begin(), input.end(), input_w);
                SetWindowText(pState->arrayOutputEdit, input_w);
            }

            delete[] array;

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

    pState->arrayInputEdit = CreateWindow(L"EDIT", L"INPUT", WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_VSCROLL, 10, 10, 700, 100, hwnd, nullptr, nullptr, nullptr);
    pState->arrayOutputEdit = CreateWindow(L"EDIT", L"OUTPUT", WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_VSCROLL, 10, 120, 700, 100, hwnd, nullptr, nullptr, nullptr);
    pState->writeFileButton = CreateButton(hwnd, L"Write", 400, 350, 100, 30, OnWriteFileButtonClicked);
    pState->readFileButton = CreateButton(hwnd, L"Read", 500, 350, 100, 30, OnReadFileButtonClicked);
    pState->writeFileAsyncButton = CreateButton(hwnd, L"Write async", 400, 380, 100, 30, OnWriteFileAsyncButtonClicked);
    pState->readFileAsyncButton = CreateButton(hwnd, L"Read async", 500, 380, 100, 30, OnReadFileAsyncButtonClicked);
    pState->timeText2 = CreateWindow(L"STATIC", L"TIME:", WS_VISIBLE | WS_CHILD, 860, 10, 40, 20, hwnd, nullptr, nullptr, nullptr);
    pState->timeText2 = CreateWindow(L"STATIC", L"", WS_VISIBLE | WS_CHILD, 900, 10, 80, 20, hwnd, nullptr, nullptr, nullptr);
    pState->arraySizeText = CreateWindow(L"STATIC", L"Array size:", WS_VISIBLE | WS_CHILD, 820, 40, 80, 20, hwnd, nullptr, nullptr, nullptr);
    pState->arraySizeInput = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD, 900, 40, 80, 20, hwnd, nullptr, nullptr, nullptr);
    pState->threadsAmountText = CreateWindow(L"STATIC", L"Threads amount:", WS_VISIBLE | WS_CHILD, 780, 70, 120, 20, hwnd, nullptr, nullptr, nullptr);
    pState->threadsAmountInput = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD, 900, 70, 20, 20, hwnd, nullptr, nullptr, nullptr);
    pState->printText = CreateWindow(L"STATIC", L"To print?", WS_VISIBLE | WS_CHILD, 830, 100, 60, 20, hwnd, nullptr, nullptr, nullptr);
    pState->printEdit = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD, 900, 100, 10, 20, hwnd, nullptr, nullptr, nullptr);
    return 0;
}

void FillArray(int* array, int size)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(-2147483648, 2147483647);
    //std::uniform_int_distribution<> distr(1, 10000);

    for (int i = 0; i < size; i++)
    {
        array[i] = distr(gen);
    }
}

void MergeSort(int* array, int left, int right)
{
    if (left == right)
    {
        return;
    }

    int mid = (left + right) / 2;
    
    MergeSort(array, left, mid);
    MergeSort(array, mid + 1, right);
    Merge(array, left, mid, right);
}

void Merge(int* array, int left, int mid, int right)
{
    int i = left;
    int j = mid + 1;

    int* temp = new int[right - left + 1];

    for (int step = 0; step < right - left + 1; step++)
    {
        if ((j > right) || ((i <= mid) && (array[i] < array[j])))
        {
            temp[step] = array[i];
            i++;
        }
        else
        {
            temp[step] = array[j];
            j++;
        }
    }

    for (int step = 0; step < right - left + 1; step++)
    {
        array[left + step] = temp[step];
    }

    delete[] temp;
}
