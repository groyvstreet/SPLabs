#ifndef UNICODE
#define UNICODE
#endif 

#include <Windows.h>
#include <chrono>
#include <string>
#include <random>
#include <thread>

const int OnSortButtonClicked = 1;

struct StateInfo
{
    HWND arrayInputEdit = {};
    HWND arrayOutputEdit = {};
    HWND sortButton = {};
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
// void MergeSort(LPVOID p);
void MergeSort(int* array, int left, int right);
void FillArray(int* array, int size);

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
        L"Multithreading sort",
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
        switch (wParam)
        {
        case OnSortButtonClicked:
        {
            TCHAR stringArraySize[] = TEXT("__________");
            GetWindowText(pState->arraySizeInput, stringArraySize, 11);
            int arraySize = _wtoi(stringArraySize);

            TCHAR stringThreadsAmount[] = TEXT("__");
            GetWindowText(pState->threadsAmountInput, stringThreadsAmount, 3);
            threadsAmount = _wtoi(stringThreadsAmount);

            TCHAR stringToPrint[] = TEXT("_");
            GetWindowText(pState->printEdit, stringToPrint, 2);
            int toPrint = _wtoi(stringToPrint);

            if (threadsAmount < 0)
            {
                threadsAmount = 0;
            }

            if (arraySize > 0)
            {
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

                Arguments params = { array, 0, arraySize - 1 };
                auto begin = std::chrono::steady_clock::now();
                //MergeSort(LPVOID(&params));
                MergeSort(array, 0, arraySize - 1);
                auto end = std::chrono::steady_clock::now();
                auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
                input = std::to_string(elapsed_ms.count());
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
            }
            else
            {
                MessageBox(hwnd, L"Invalid array size!", L"Invalid input", MB_OK);
            }
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
    RECT rect;
    GetClientRect(hwnd, &rect);
    auto pState = GetAppState(hwnd);

    pState->arrayInputEdit = CreateWindow(L"EDIT", L"INPUT", WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_VSCROLL, 10, 10, 700, 100, hwnd, nullptr, nullptr, nullptr);
    pState->arrayOutputEdit = CreateWindow(L"EDIT", L"OUTPUT", WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_VSCROLL, 10, 120, 700, 100, hwnd, nullptr, nullptr, nullptr);
    pState->sortButton = CreateButton(hwnd, L"Sort", 400, 400, 200, 30, OnSortButtonClicked);
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

void MergeSort(int* array, int left, int right)
{
    // Arguments* params = (Arguments*)p;
    // int* array = params->array;
    // int left = params->left;
    // int right = params->right;

    if (left == right)
    {
        return;
    }

    int mid = (left + right) / 2;
    Arguments p1 = { array, left, mid };
    Arguments p2 = { array, mid + 1, right };

    if (threadsAmount > 0)
    {
        threadsAmount -= 1;
        std::thread thread(MergeSort, array, left, mid);
        MergeSort(array, mid + 1, right);
        thread.join();
        // HANDLE thread1 = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MergeSort, &p1, 0, 0);
        // HANDLE thread2 = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MergeSort, &p2, 0, 0);
        // WaitForSingleObject(thread1, INFINITE);
        // WaitForSingleObject(thread2, INFINITE);
        // CloseHandle(thread1);
        // CloseHandle(thread2);
        
    }
    else
    {
        MergeSort(array, left, mid);
        MergeSort(array, mid + 1, right);
        // MergeSort(&p1);
        // MergeSort(&p2);
    }

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

void FillArray(int* array, int size)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(-2147483648, 2147483647);

    for (int i = 0; i < size; i++)
    {
        array[i] = distr(gen);
    }
}
