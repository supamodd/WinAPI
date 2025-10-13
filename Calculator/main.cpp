#include <Windows.h>
#include "resource.h"
#include <string>
#include <sstream>
#include <iostream>

CONST CHAR g_sz_CLASS_NAME[] = "Calc_SPU_411";
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Прототипы функций
void UpdateDisplay(HWND hwndDisplay, const std::string& value);
void ProcessButtonClick(HWND hwndDisplay, WPARAM wParam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
    WNDCLASSEX wClass;
    ZeroMemory(&wClass, sizeof(wClass));

    wClass.style = 0;
    wClass.cbSize = sizeof(wClass);
    wClass.cbWndExtra = 0;
    wClass.cbClsExtra = 0;

    wClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wClass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wClass.hbrBackground = (HBRUSH)COLOR_WINDOW;

    wClass.hInstance = hInstance;
    wClass.lpfnWndProc = WndProc;
    wClass.lpszMenuName = NULL;
    wClass.lpszClassName = g_sz_CLASS_NAME;

    if (!RegisterClassEx(&wClass))
    {
        MessageBox(NULL, "Class registration failed", "", MB_OK | MB_ICONERROR);
        return 0;
    }

    HWND hwnd = CreateWindowEx
    (
        NULL,
        g_sz_CLASS_NAME,
        g_sz_CLASS_NAME,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        300, 400,
        NULL,
        NULL,
        hInstance,
        NULL
    );
    if (hwnd == NULL)
    {
        MessageBox(NULL, "Window creation failed", "Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // 3) Запуск цикла сообщений:
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static HWND hwndDisplay;

    switch (uMsg)
    {
    case WM_CREATE:
        hwndDisplay = CreateWindowEx(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_READONLY,
            10, 10, 260, 30, hwnd, (HMENU)IDC_EDIT_DISPLAY, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

        // Кнопки калькулятора
        CreateWindow("BUTTON", "7", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 50, 60, 40, hwnd, (HMENU)IDC_BUTTON_7, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        CreateWindow("BUTTON", "8", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 80, 50, 60, 40, hwnd, (HMENU)IDC_BUTTON_8, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        CreateWindow("BUTTON", "9", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 150, 50, 60, 40, hwnd, (HMENU)IDC_BUTTON_9, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        CreateWindow("BUTTON", "/", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 220, 50, 60, 40, hwnd, (HMENU)IDC_BUTTON_SLASH, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

        // Остальные кнопки
        CreateWindow("BUTTON", "4", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 100, 60, 40, hwnd, (HMENU)IDC_BUTTON_4, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        CreateWindow("BUTTON", "5", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 80, 100, 60, 40, hwnd, (HMENU)IDC_BUTTON_5, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        CreateWindow("BUTTON", "6", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 150, 100, 60, 40, hwnd, (HMENU)IDC_BUTTON_6, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        CreateWindow("BUTTON", "*", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 220, 100, 60, 40, hwnd, (HMENU)IDC_BUTTON_ASTER, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

        CreateWindow("BUTTON", "1", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 150, 60, 40, hwnd, (HMENU)IDC_BUTTON_1, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        CreateWindow("BUTTON", "2", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 80, 150, 60, 40, hwnd, (HMENU)IDC_BUTTON_2, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        CreateWindow("BUTTON", "3", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 150, 150, 60, 40, hwnd, (HMENU)IDC_BUTTON_3, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        CreateWindow("BUTTON", "-", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 220, 150, 60, 40, hwnd, (HMENU)IDC_BUTTON_MINUS, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

        CreateWindow("BUTTON", "0", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 200, 60, 40, hwnd, (HMENU)IDC_BUTTON_0, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        CreateWindow("BUTTON", ".", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 80, 200, 60, 40, hwnd, (HMENU)IDC_BUTTON_POINT, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        CreateWindow("BUTTON", "=", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 150, 200, 60, 40, hwnd, (HMENU)IDC_BUTTON_EQUAL, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        CreateWindow("BUTTON", "+", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 220, 200, 60, 40, hwnd, (HMENU)IDC_BUTTON_PLUS, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

        CreateWindow("BUTTON", "CLR", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 250, 60, 40, hwnd, (HMENU)IDC_BUTTON_CLR, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        CreateWindow("BUTTON", "BSP", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 80, 250, 60, 40, hwnd, (HMENU)IDC_BUTTON_BSP, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) >= IDC_BUTTON_0 && LOWORD(wParam) <= IDC_BUTTON_EQUAL)
        {
            ProcessButtonClick(hwndDisplay, wParam);

        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_CLOSE:
        SendMessage(hwnd, WM_DESTROY, 0, 0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

void UpdateDisplay(HWND hwndDisplay, const std::string& value)
{
    SetWindowText(hwndDisplay, value.c_str());
}

void ProcessButtonClick(HWND hwndDisplay, WPARAM wParam)
{
    static std::string currentValue = "";                   // Текущее значение, которое выводится
    static std::string currentOperation = "";           // Операция
    static double result = 0.0;                                  // Результат вычислений
    static bool operationPressed = false;

    try {
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON_0: currentValue += "0"; break;
        case IDC_BUTTON_1: currentValue += "1"; break;
        case IDC_BUTTON_2: currentValue += "2"; break;
        case IDC_BUTTON_3: currentValue += "3"; break;
        case IDC_BUTTON_4: currentValue += "4"; break;
        case IDC_BUTTON_5: currentValue += "5"; break;
        case IDC_BUTTON_6: currentValue += "6"; break;
        case IDC_BUTTON_7: currentValue += "7"; break;
        case IDC_BUTTON_8: currentValue += "8"; break;
        case IDC_BUTTON_9: currentValue += "9"; break;
        case IDC_BUTTON_POINT: currentValue += "."; break;
        case IDC_BUTTON_PLUS:
            if (!operationPressed) {
                result = std::stod(currentValue);
                currentValue = "";
                currentOperation = "+";
                operationPressed = true;
            }
            break;
        case IDC_BUTTON_MINUS:
            if (!operationPressed)
            {
                result = std::stod(currentValue);
                currentValue = "";
                currentOperation = "-";
                operationPressed = true;
            }
            break;
        case IDC_BUTTON_SLASH:
            if (!operationPressed)
            {
                result = std::stod(currentValue);
                currentValue = "";
                currentOperation = "/";
                operationPressed = true;
            }
            break;
        case IDC_BUTTON_ASTER:
            if (!operationPressed) 
            {
                result = std::stod(currentValue);
                currentValue = "";
                currentOperation = "*";
                operationPressed = true;
            }
            break;
        case IDC_BUTTON_CLR:
            currentValue = "";
            result = 0;
            currentOperation = "";
            operationPressed = false;
            break;
        case IDC_BUTTON_BSP:
            if (!currentValue.empty())
            {
                currentValue.pop_back();
            }
            break;
        case IDC_BUTTON_EQUAL:
            if (currentOperation == "+")
            {
                result += std::stod(currentValue);
            }
            else if (currentOperation == "-") 
            {
                result -= std::stod(currentValue);
            }
            else if (currentOperation == "*")
            {
                result *= std::stod(currentValue);
            }
            else if (currentOperation == "/")
            {
                if (currentValue == "0")
                {
                    currentValue = "Error";
                    UpdateDisplay(hwndDisplay, currentValue);
                    return;
                }
                result /= std::stod(currentValue);
            }
            currentValue = std::to_string(result);
            operationPressed = false;
            break;
        }

        UpdateDisplay(hwndDisplay, currentValue);
    }
    catch (...)
    {
        currentValue = "Error"; 
        UpdateDisplay(hwndDisplay, currentValue);
    }
}