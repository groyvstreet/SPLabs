#ifndef UNICODE
#define UNICODE
#endif

#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#pragma comment(lib, "ws2_32.lib")

const int OnFindKeyButtonClicked = 1;
const int OnFindValueButtonClicked = 2;

TCHAR TYPE0[] = L"REG_NONE";
TCHAR TYPE1[] = L"REG_SZ";
TCHAR TYPE2[] = L"REG_EXPAND_SZ";
TCHAR TYPE3[] = L"REG_BINARY";
TCHAR TYPE4[] = L"REG_DWORD";
TCHAR TYPE5[] = L"REG_DWORD_BIG_ENDIAN";
TCHAR TYPE6[] = L"REG_LINK";
TCHAR TYPE7[] = L"REG_MULTI_SZ";
TCHAR TYPE8[] = L"REG_RESOURCE_LIST";
TCHAR TYPE9[] = L"UNDEFINED";

struct StateInfo
{
    HWND keyOutput = {};
    HWND valueOutput = {};
    HWND keyInput = {};
    HWND valueInput = {};
    HWND findKeyButton = {};
    HWND findValueButton = {};
};

struct Array { BYTE *array = (BYTE*)calloc(64, sizeof(BYTE)); };

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
StateInfo* GetAppState(HWND hwnd);
HWND CreateButton(HWND hwnd, LPCWSTR buttonText, int x, int y, int width, int height, int OnButtonClicked);
LRESULT Initialize(HWND hwnd);
HKEY FindKey(HKEY hKey, LPCWSTR name);
LSTATUS GetValue(HKEY hKey, DWORD index, std::vector<LPCSTR>* valuesNames, std::vector<DWORD>* valuesTypes, std::vector<Array>* valuesData);
bool isContainSlash(LPWSTR string, int* slashIndex);
void FindValue(HKEY hKey, LPWSTR keyName, LPCSTR valueName, std::ofstream* file);
LSTATUS GetValue(HKEY hKey, DWORD index, LPSTR ipValueName, LPDWORD ipType, LPBYTE ipData);

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
        L"Lab8",
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

        case OnFindKeyButtonClicked:
        {
            HKEY hKey = NULL;

            LPWSTR forInput = (LPWSTR)calloc(256, sizeof(WCHAR));

            GetWindowText(pState->keyInput, forInput, 256);

            int slashIndex;
            bool skip = false;
            bool first = true;

            while (isContainSlash(forInput, &slashIndex) == true)
            {
                LPWSTR temp = (LPWSTR)calloc(256, sizeof(WCHAR));

                for (int i = 0; i < slashIndex; i++)
                {
                    temp[i] = forInput[i];

                    if (i == slashIndex - 1)
                    {
                        temp[++i] = '\0';
                    }
                }

                for (int i = 0; i < 256; i++)
                {
                    forInput[i] = forInput[i + slashIndex + 1];

                    if (forInput[i + slashIndex + 1] == '\0')
                    {
                        break;
                    }
                }

                if (wcscmp(temp, L"HKEY_CLASSES_ROOT") == 0)
                {
                    if (isContainSlash(forInput, &slashIndex) == true)
                    {
                        for (int i = 0; i < slashIndex; i++)
                        {
                            temp[i] = forInput[i];

                            if (i == slashIndex - 1)
                            {
                                temp[++i] = '\0';
                            }
                        }

                        for (int i = 0; i < 256; i++)
                        {
                            forInput[i] = forInput[i + slashIndex + 1];

                            if (forInput[i + slashIndex + 1] == '\0')
                            {
                                break;
                            }
                        }

                        hKey = FindKey(HKEY_CLASSES_ROOT, temp);
                    }
                    else
                    {
                        hKey = FindKey(HKEY_CLASSES_ROOT, forInput);
                    }

                    first = false;
                    continue;
                }
                else if (wcscmp(temp, L"HKEY_CURRENT_USER") == 0)
                {
                    if (isContainSlash(forInput, &slashIndex) == true)
                    {
                        for (int i = 0; i < slashIndex; i++)
                        {
                            temp[i] = forInput[i];

                            if (i == slashIndex - 1)
                            {
                                temp[++i] = '\0';
                            }
                        }

                        for (int i = 0; i < 256; i++)
                        {
                            forInput[i] = forInput[i + slashIndex + 1];

                            if (forInput[i + slashIndex + 1] == '\0')
                            {
                                break;
                            }
                        }

                        hKey = FindKey(HKEY_CURRENT_USER, temp);
                    }
                    else
                    {
                        hKey = FindKey(HKEY_CURRENT_USER, forInput);
                    }

                    first = false;
                    continue;
                }
                else if (wcscmp(temp, L"HKEY_LOCAL_MACHINE") == 0)
                {
                    if (isContainSlash(forInput, &slashIndex) == true)
                    {
                        for (int i = 0; i < slashIndex; i++)
                        {
                            temp[i] = forInput[i];

                            if (i == slashIndex - 1)
                            {
                                temp[++i] = '\0';
                            }
                        }

                        for (int i = 0; i < 256; i++)
                        {
                            forInput[i] = forInput[i + slashIndex + 1];

                            if (forInput[i + slashIndex + 1] == '\0')
                            {
                                break;
                            }
                        }

                        hKey = FindKey(HKEY_LOCAL_MACHINE, temp);
                    }
                    else
                    {
                        hKey = FindKey(HKEY_LOCAL_MACHINE, forInput);
                    }

                    first = false;
                    continue;
                }
                else if (wcscmp(temp, L"HKEY_USERS") == 0)
                {
                    if (isContainSlash(forInput, &slashIndex) == true)
                    {
                        for (int i = 0; i < slashIndex; i++)
                        {
                            temp[i] = forInput[i];

                            if (i == slashIndex - 1)
                            {
                                temp[++i] = '\0';
                            }
                        }

                        for (int i = 0; i < 256; i++)
                        {
                            forInput[i] = forInput[i + slashIndex + 1];

                            if (forInput[i + slashIndex + 1] == '\0')
                            {
                                break;
                            }
                        }

                        hKey = FindKey(HKEY_USERS, temp);
                    }
                    else
                    {
                        hKey = FindKey(HKEY_USERS, forInput);
                    }

                    first = false;
                    continue;
                }
                else if (wcscmp(temp, L"HKEY_CURRENT_CONFIG") == 0)
                {
                    if (isContainSlash(forInput, &slashIndex) == true)
                    {
                        for (int i = 0; i < slashIndex; i++)
                        {
                            temp[i] = forInput[i];

                            if (i == slashIndex - 1)
                            {
                                temp[++i] = '\0';
                            }
                        }

                        for (int i = 0; i < 256; i++)
                        {
                            forInput[i] = forInput[i + slashIndex + 1];

                            if (forInput[i + slashIndex + 1] == '\0')
                            {
                                break;
                            }
                        }

                        hKey = FindKey(HKEY_CURRENT_CONFIG, temp);
                    }
                    else
                    {
                        hKey = FindKey(HKEY_CURRENT_CONFIG, forInput);
                    }

                    first = false;
                    continue;
                }

                if (first == true)
                {
                    hKey = FindKey(HKEY_CLASSES_ROOT, temp);

                    if (hKey == NULL)
                    {
                        hKey = FindKey(HKEY_CURRENT_USER, temp);
                    }

                    if (hKey == NULL)
                    {
                        hKey = FindKey(HKEY_LOCAL_MACHINE, temp);
                    }

                    if (hKey == NULL)
                    {
                        hKey = FindKey(HKEY_USERS, temp);
                    }

                    if (hKey == NULL)
                    {
                        hKey = FindKey(HKEY_CURRENT_CONFIG, temp);
                    }

                    if (hKey == NULL)
                    {
                        skip = true;
                        break;
                    }

                    first = false;
                }
                else
                {
                    hKey = FindKey(hKey, temp);

                    if (hKey == NULL)
                    {
                        break;
                    }
                }

                //free(temp);
            }

            if (hKey != NULL)
            {
                skip = true;
                hKey = FindKey(hKey, forInput);
            }

            if (skip == false)
            {
                if (wcscmp(forInput, L"HKEY_CURRENT_CONFIG") == 0)
                {
                    hKey = HKEY_CURRENT_CONFIG;
                }
                else if (wcscmp(forInput, L"HKEY_USERS") == 0)
                {
                    hKey = HKEY_USERS;
                }
                else if (wcscmp(forInput, L"HKEY_LOCAL_MACHINE") == 0)
                {
                    hKey = HKEY_LOCAL_MACHINE;
                }
                else if (wcscmp(forInput, L"HKEY_CURRENT_USER") == 0)
                {
                    hKey = HKEY_CURRENT_USER;
                }
                else if (wcscmp(forInput, L"HKEY_CLASSES_ROOT") == 0)
                {
                    hKey = HKEY_CLASSES_ROOT;
                }

                if (hKey == NULL)
                {
                    hKey = FindKey(HKEY_CLASSES_ROOT, forInput);
                }

                if (hKey == NULL)
                {
                    hKey = FindKey(HKEY_CURRENT_USER, forInput);
                }

                if (hKey == NULL)
                {
                    hKey = FindKey(HKEY_LOCAL_MACHINE, forInput);
                }

                if (hKey == NULL)
                {
                    hKey = FindKey(HKEY_USERS, forInput);
                }

                if (hKey == NULL)
                {
                    hKey = FindKey(HKEY_CURRENT_CONFIG, forInput);
                }
            }

            if (hKey == NULL)
            {
                SetWindowText(pState->keyOutput, L"Not found!");
            }
            else
            {
                SetWindowText(pState->keyOutput, L"Successfully found!");

                DWORD index = 0;
                LPWSTR IpName = (LPWSTR)calloc(256, sizeof(WCHAR));
                std::vector<LPCWSTR> keys;

                while (RegEnumKey(hKey, index, IpName, 256) == ERROR_SUCCESS)
                {
                    LPWSTR dest = (LPWSTR)calloc(256, sizeof(WCHAR));
                    wcscpy(dest, IpName);
                    keys.push_back(dest);
                    ++index;
                    //free(dest);
                }

                //free(IpName);

                LPWSTR forOutput = (LPWSTR)calloc(256 * keys.size(), sizeof(WCHAR));

                int i = 0;

                for (int j = 0; j < keys.size(); j++)
                {
                    for (int k = 0; k < 256; k++)
                    {
                        if (keys[j][k] == '\0')
                        {
                            forOutput[i] = '\r';
                            forOutput[++i] = '\n';
                            i++;
                            break;
                        }

                        forOutput[i] = keys[j][k];
                        i++;
                    }
                }

                forOutput[i] = '\0';
                SetWindowText(pState->keyOutput, forOutput);

                //free(forOutput);

                index = 0;
                std::vector<LPCSTR> valuesNames;
                std::vector<DWORD> valuesTypes;
                std::vector<Array> valuesData;

                while (GetValue(hKey, index, &valuesNames, &valuesTypes, &valuesData) == ERROR_SUCCESS)
                {
                    index++;
                }

                forOutput = (LPWSTR)calloc(256 * valuesNames.size(), sizeof(WCHAR));

                i = 0;

                for (int j = 0; j < valuesNames.size(); j++)
                {
                    for (int k = 0; k < 256; k++)
                    {
                        if (valuesNames[j][k] == '\0')
                        {
                            forOutput[i++] = '\t';

                            switch (valuesTypes[j])
                            {
                            case 0: {
                                for (int l = 0; l < 8; l++)
                                {
                                    forOutput[i] = TYPE0[l];
                                    i++;
                                }
                                break;
                            }
                            case 1: {
                                for (int l = 0; l < 6; l++)
                                {
                                    forOutput[i] = TYPE1[l];
                                    i++;
                                }

                                forOutput[i++] = '\t';

                                wchar_t WArray[64];
                                mbstowcs(WArray, (char*)valuesData[j].array, _countof(WArray));

                                for (int l = 0; l < 64; l++)
                                {
                                    if (WArray[l] == '\0')
                                    {
                                        break;
                                    }

                                    forOutput[i] = WArray[l];
                                    i++;
                                }

                                break;
                            }
                            case 2: {
                                for (int l = 0; l < 13; l++)
                                {
                                    forOutput[i] = TYPE2[l];
                                    i++;
                                }

                                forOutput[i++] = '\t';

                                wchar_t WArray[64];
                                mbstowcs(WArray, (char*)valuesData[j].array, _countof(WArray));

                                for (int l = 0; l < 64; l++)
                                {
                                    if (WArray[l] == '\0')
                                    {
                                        break;
                                    }

                                    forOutput[i] = WArray[l];
                                    i++;
                                }

                                break;
                            }
                            case 3: {
                                for (int l = 0; l < 10; l++)
                                {
                                    forOutput[i] = TYPE3[l];
                                    i++;
                                }

                                forOutput[i++] = '\t';

                                int ints[64];

                                for (int l = 0; l < 64; l++)
                                {
                                    ints[l] = valuesData[j].array[l];
                                    std::wstring s = std::to_wstring(ints[l]);

                                    for (int p = 0; p < s.length(); p++)
                                    {
                                        forOutput[i] = s[p];
                                        i++;
                                    }

                                    forOutput[i++] = ' ';
                                }

                                break;
                            }
                            case 4: {
                                for (int l = 0; l < 9; l++)
                                {
                                    forOutput[i] = TYPE4[l];
                                    i++;
                                }

                                forOutput[i++] = '\t';

                                DWORD dword = *(DWORD*)valuesData[j].array;

                                wchar_t  str[64];
                                _itow(dword, str, 10);

                                for (int l = 0; l < 256; l++)
                                {
                                    if (str[l] == '\0')
                                    {
                                        break;
                                    }

                                    forOutput[i] = str[l];
                                    i++;
                                }

                                break;
                            }
                            case 5: {
                                for (int l = 0; l < 20; l++)
                                {
                                    forOutput[i] = TYPE5[l];
                                    i++;
                                }

                                forOutput[i++] = '\t';

                                DWORD dword = *(DWORD*)valuesData[j].array;

                                wchar_t  str[64];
                                _itow(dword, str, 10);

                                for (int l = 0; l < 256; l++)
                                {
                                    if (str[l] == '\0')
                                    {
                                        break;
                                    }

                                    forOutput[i] = str[l];
                                    i++;
                                }

                                break;
                            }
                            case 6: {
                                for (int l = 0; l < 8; l++)
                                {
                                    forOutput[i] = TYPE6[l];
                                    i++;
                                }
                                break;
                            }
                            case 7: {
                                for (int l = 0; l < 12; l++)
                                {
                                    forOutput[i] = TYPE7[l];
                                    i++;
                                }
                                break;
                            }
                            case 8: {
                                for (int l = 0; l < 17; l++)
                                {
                                    forOutput[i] = TYPE8[l];
                                    i++;
                                }
                                break;
                            }
                            default: {
                                for (int l = 0; l < 9; l++)
                                {
                                    forOutput[i] = TYPE9[l];
                                    i++;
                                }
                            }
                            }

                            forOutput[i] = '\r';
                            forOutput[++i] = '\n';
                            i++;
                            break;
                        }

                        forOutput[i] = valuesNames[j][k];
                        i++;
                    }
                }

                forOutput[i] = '\0';
                SetWindowText(pState->valueOutput, forOutput);
                //free(forOutput);

                for (int i = 0; i < valuesData.size(); i++)
                {
                    //free(valuesData[i].array);
                }
            }

            //free(forInput);

            return 0;
        }

        case OnFindValueButtonClicked:
        {
            LPWSTR forInput = (LPWSTR)calloc(256, sizeof(WCHAR));

            GetWindowText(pState->valueInput, forInput, 256);

            LPSTR valueName = (LPSTR)calloc(256, sizeof(TCHAR));

            for (int i = 0; i < 256; i++)
            {
                valueName[i] = forInput[i];
            }

            LPWSTR forOutput = (LPWSTR)calloc(256, sizeof(WCHAR));

            std::ofstream fout;
            fout.open("output.txt");

            int index = 0;

            FindValue(HKEY_CLASSES_ROOT, (LPWSTR)L"HKEY_CLASSES_ROOT", valueName, &fout);
            FindValue(HKEY_CURRENT_USER, (LPWSTR)L"HKEY_CURRENT_USER", valueName, &fout);
            FindValue(HKEY_LOCAL_MACHINE, (LPWSTR)L"HKEY_LOCAL_MACHINE", valueName, &fout);
            FindValue(HKEY_USERS, (LPWSTR)L"HKEY_USERS", valueName, &fout);
            FindValue(HKEY_CURRENT_CONFIG, (LPWSTR)L"HKEY_CURRENT_CONFIG", valueName, &fout);

            fout.close();

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

    pState->keyOutput = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_VSCROLL, 10, 10, 480, 300, hwnd, nullptr, nullptr, nullptr);
    pState->valueOutput = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_VSCROLL, 500, 10, 480, 300, hwnd, nullptr, nullptr, nullptr);
    pState->keyInput = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_VSCROLL, 10, 320, 970, 40, hwnd, nullptr, nullptr, nullptr);
    pState->valueInput = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_VSCROLL, 10, 370, 970, 40, hwnd, nullptr, nullptr, nullptr);
    pState->findKeyButton = CreateButton(hwnd, L"Find key", 390, 430, 100, 20, OnFindKeyButtonClicked);
    pState->findValueButton = CreateButton(hwnd, L"Find value", 510, 430, 100, 20, OnFindValueButtonClicked);

    return 0;
}

HKEY FindKey(HKEY hKey, LPCWSTR name)
{
    DWORD index = 0;
    LPWSTR IpName = (LPWSTR)calloc(256, sizeof(WCHAR));

    while (RegEnumKey(hKey, index, IpName, 256) == ERROR_SUCCESS)
    {
        if (wcscmp(name, IpName) == 0)
        {
            HKEY hKeyToReturn = NULL;
            RegOpenKey(hKey, IpName, &hKeyToReturn);
            //free(IpName);
            return hKeyToReturn;
        }
        else
        {
            HKEY hKeyForNext = NULL;
            RegOpenKey(hKey, IpName, &hKeyForNext);
            //free(IpName);

            HKEY hKeyToReturn = NULL;
            hKeyToReturn = FindKey(hKeyForNext, name);

            if (hKeyToReturn != NULL)
            {
                return hKeyToReturn;
            }
        }

        ++index;
    }

    return NULL;
}

LSTATUS GetValue(HKEY hKey, DWORD index, std::vector<LPCSTR> *valuesNames, std::vector<DWORD> *valuesTypes, std::vector<Array> *valuesData)
{
    LPSTR IpValueName = (LPSTR)calloc(256, sizeof(TCHAR));
    DWORD IpcbValueName;
    DWORD IpType;
    BYTE* IpData = (BYTE*)calloc(256, sizeof(BYTE));
    DWORD IpcbData = 256;

    auto msg = RegEnumValueA(hKey, index, IpValueName, &IpcbValueName, NULL, &IpType, IpData, &IpcbData);

    if (msg == ERROR_SUCCESS)
    {
        LPSTR nameCopy = (LPSTR)calloc(256, sizeof(TCHAR));
        strcpy(nameCopy, IpValueName);
        valuesNames->push_back(nameCopy);
        valuesTypes->push_back(IpType);
        Array array = { IpData };
        valuesData->push_back(array);
    }

    //free(IpValueName);

    return msg;
}

bool isContainSlash(LPWSTR string, int *slashIndex)
{
    bool isContain = false;

    for (int i = 0; i < 256; i++)
    {
        if (string[i] == '/')
        {
            isContain = true;
            *slashIndex = i;
            break;
        }

        if (string[i] == '\0')
        {
            break;
        }
    }

    return isContain;
}

void FindValue(HKEY hKey, LPWSTR keyName, LPCSTR valueName, std::ofstream *file)
{
    DWORD index = 0;
    LPSTR IpValueName = (LPSTR)calloc(256, sizeof(TCHAR));
    DWORD IpType;
    BYTE* IpData = (BYTE*)calloc(256, sizeof(BYTE));

    LPWSTR str = (LPWSTR)calloc(256, sizeof(WCHAR));
    int i = 0;

    while (GetValue(hKey, index, IpValueName, &IpType, IpData) == ERROR_SUCCESS)
    {
        if (strcmp(IpValueName, valueName) == 0)
        {
            for (int j = 0; j < 256; j++)
            {
                if (keyName[j] == '\0')
                {
                    break;
                }

                str[i] = keyName[j];
                i++;
            }

            str[i] = ':';
            i++;
            str[i] = '\t';
            i++;

            switch (IpType)
            {
            case 0: {
                for (int l = 0; l < 8; l++)
                {
                    str[i] = TYPE0[l];
                    i++;
                }
                break;
            }
            case 1: {
                for (int l = 0; l < 6; l++)
                {
                    str[i] = TYPE1[l];
                    i++;
                }

                str[i] = '\t';
                i++;

                wchar_t WArray[64];
                mbstowcs(WArray, (char*)IpData, _countof(WArray));

                for (int l = 0; l < 64; l++)
                {
                    if (WArray[l] == '\0')
                    {
                        break;
                    }

                    str[i] = WArray[l];
                    i++;
                }

                break;
            }
            case 2: {
                for (int l = 0; l < 13; l++)
                {
                    str[i] = TYPE2[l];
                    i++;
                }

                str[i] = '\t';
                i++;

                wchar_t WArray[64];
                mbstowcs(WArray, (char*)IpData, _countof(WArray));

                for (int l = 0; l < 64; l++)
                {
                    if (WArray[l] == '\0')
                    {
                        break;
                    }

                    str[i] = WArray[l];
                    i++;
                }

                break;
            }
            case 3: {
                for (int l = 0; l < 10; l++)
                {
                    str[i] = TYPE3[l];
                    i++;
                }

                str[i] = '\t';
                i++;

                int ints[64];

                for (int l = 0; l < 64; l++)
                {
                    ints[l] = IpData[l];
                    std::wstring s = std::to_wstring(ints[l]);

                    for (int p = 0; p < s.length(); p++)
                    {
                        str[i] = s[p];
                        i++;
                    }

                    str[i] = ' ';
                    i++;
                }

                break;
            }
            case 4: {
                for (int l = 0; l < 9; l++)
                {
                    str[i] = TYPE4[l];
                    i++;
                }

                str[i] = '\t';
                i++;

                DWORD dword = *(DWORD*)IpData;

                wchar_t  str[64];
                _itow(dword, str, 10);

                for (int l = 0; l < 256; l++)
                {
                    if (str[l] == '\0')
                    {
                        break;
                    }

                    str[i] = str[l];
                    i++;
                }

                break;
            }
            case 5: {
                for (int l = 0; l < 20; l++)
                {
                    str[i] = TYPE5[l];
                    i++;
                }

                str[i] = '\t';
                i++;

                DWORD dword = *(DWORD*)IpData;

                wchar_t  str[64];
                _itow(dword, str, 10);

                for (int l = 0; l < 256; l++)
                {
                    if (str[l] == '\0')
                    {
                        break;
                    }

                    str[i] = str[l];
                    i++;
                }

                break;
            }
            case 6: {
                for (int l = 0; l < 8; l++)
                {
                    str[i] = TYPE6[l];
                    i++;
                }
                break;
            }
            case 7: {
                for (int l = 0; l < 12; l++)
                {
                    str[i] = TYPE7[l];
                    i++;
                }
                break;
            }
            case 8: {
                for (int l = 0; l < 17; l++)
                {
                    str[i] = TYPE8[l];
                    i++;
                }
                break;
            }
            default: {
                for (int l = 0; l < 9; l++)
                {
                    str[i] = TYPE9[l];
                    i++;
                }
            }
            }

            str[i] = '\r';
            i++;
            str[i] = '\n';
            i++;

            std::wstring ws(str);
            std::string toFile = std::string(ws.begin(), ws.end());
            *file << toFile;
        }

        index++;
    }

    free(str);

    index = 0;
    LPWSTR IpName = (LPWSTR)calloc(256, sizeof(WCHAR));

    while (RegEnumKey(hKey, index, IpName, 256) == ERROR_SUCCESS)
    {
        HKEY hKeyForNext = NULL;
        RegOpenKey(hKey, IpName, &hKeyForNext);
        FindValue(hKeyForNext, IpName, valueName, file);
        ++index;
    }
}

LSTATUS GetValue(HKEY hKey, DWORD index, LPSTR ipValueName, LPDWORD ipType, LPBYTE ipData)
{
    LPSTR IpValueName = (LPSTR)calloc(256, sizeof(TCHAR));
    DWORD IpcbValueName;
    DWORD IpType;
    BYTE* IpData = (BYTE*)calloc(256, sizeof(BYTE));
    DWORD IpcbData = 256;

    auto msg = RegEnumValueA(hKey, index, IpValueName, &IpcbValueName, NULL, &IpType, IpData, &IpcbData);

    if (msg == ERROR_SUCCESS)
    {
        LPSTR nameCopy = (LPSTR)calloc(256, sizeof(TCHAR));
        strcpy(nameCopy, IpValueName);
        strcpy(ipValueName, IpValueName);
        *ipType = IpType;
        memcpy(ipData, IpData, 256);
    }

    //free(IpValueName);

    return msg;
}
