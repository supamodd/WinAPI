#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <Windows.h>
#include "resource.h"
#include <float.h>

CONST CHAR g_sz_CLASS_NAME[] = "Calc_CPU_411";

CONST INT g_i_START_X = 10;
CONST INT g_i_START_Y = 10;

CONST INT g_i_BUTTON_SIZE = 50;
CONST INT g_i_INTERVAL = 3;
CONST INT g_i_BUTTON_DOUBLE_SIZE = g_i_BUTTON_SIZE * 2 + g_i_INTERVAL;

CONST INT g_i_SCREEN_WIDTH = (g_i_BUTTON_SIZE + g_i_INTERVAL) * 5 - g_i_INTERVAL;
CONST INT g_i_SCREEN_HEIGHT = g_i_BUTTON_SIZE;

CONST INT g_i_BUTTON_START_X = g_i_START_X;
CONST INT g_i_BUTTON_START_Y = g_i_START_Y + g_i_SCREEN_HEIGHT + g_i_INTERVAL;

CONST CHAR g_OPERATIONS[] = "+-*/";
CONST INT g_i_OPERATIONS_START_X = g_i_BUTTON_START_X + (g_i_BUTTON_SIZE + g_i_INTERVAL) * 3;

#define BUTTON_SHIFT_X(shift) g_i_BUTTON_START_X + (g_i_BUTTON_SIZE + g_i_INTERVAL) * (shift)
#define BUTTON_SHIFT_Y(shift) g_i_BUTTON_START_Y + (g_i_BUTTON_SIZE + g_i_INTERVAL) * (shift)

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR plCmdLine, INT nCmdShow)
{
    WNDCLASSEX wClass;
    ZeroMemory(&wClass, sizeof(wClass));

    wClass.style = 0;
    wClass.cbSize = sizeof(wClass);
    wClass.cbWndExtra = 0;
    wClass.cbClsExtra = 0;

    wClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
    wClass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
    wClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wClass.hbrBackground = (HBRUSH)COLOR_WINDOW;

    wClass.hInstance = hInstance;
    wClass.lpfnWndProc = WndProc;
    wClass.lpszMenuName = NULL;
    wClass.lpszClassName = g_sz_CLASS_NAME;

    if (!RegisterClassEx(&wClass))
    {
        MessageBox(NULL, "Class reg fail", "", MB_OK | MB_ICONERROR);
        return 0;
    }

    HWND hwnd = CreateWindowEx
    (
        NULL,
        g_sz_CLASS_NAME,
        g_sz_CLASS_NAME,
        WS_OVERLAPPEDWINDOW - WS_THICKFRAME - WS_MAXIMIZEBOX,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        g_i_SCREEN_WIDTH + 2 * g_i_START_X + 16,
        g_i_BUTTON_START_Y + (g_i_BUTTON_SIZE + g_i_INTERVAL) * 4 + 42,
        NULL,
        NULL,
        hInstance,
        NULL
    );
    if (hwnd == NULL)
    {
        MessageBox(NULL, "Window fail", "Error", MB_OK | MB_ICONERROR);
        return 0;
    }
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

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
    static double result = 0; // Текущее значение
    static double current_num = 0; // Текущее вводимое число
    static double b = DBL_MIN; // Переменная для второго числа
    static int operation = 0; // Операция
    static BOOL input = FALSE; // Ввод числа
    static BOOL input_operation = FALSE; // Ввод операции

    switch (uMsg)
    {
    case WM_CREATE:
    {
        HWND hEdit = CreateWindowEx
        (
            NULL, "Edit", "0",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT,
            10, 10,
            g_i_SCREEN_WIDTH, g_i_SCREEN_HEIGHT,
            hwnd,
            (HMENU)IDC_EDIT,
            GetModuleHandle(NULL),
            NULL
        );

        INT digit = 0;
        CHAR sz_digit[2] = "";
        for (int i = 6; i >= 0; i -= 3)
        {
            for (int j = 0; j < 3; j++)
            {
                sz_digit[0] = digit + '0';
                CreateWindowEx
                (
                    NULL, "Button", sz_digit,
                    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    g_i_BUTTON_START_X + (g_i_BUTTON_SIZE + g_i_INTERVAL) * j,
                    g_i_BUTTON_START_Y + (g_i_BUTTON_SIZE + g_i_INTERVAL) * i / 3,
                    g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
                    hwnd,
                    (HMENU)(IDC_BUTTON_0 + digit),
                    GetModuleHandle(NULL),
                    NULL
                );
                digit++;
            }
        }

        CreateWindowEx
        (
            NULL, "Button", "0",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            g_i_BUTTON_START_X, BUTTON_SHIFT_Y(3),
            g_i_BUTTON_DOUBLE_SIZE, g_i_BUTTON_SIZE,
            hwnd,
            (HMENU)IDC_BUTTON_0,
            GetModuleHandle(NULL),
            NULL
        );

        CreateWindowEx
        (
            NULL, "Button", ".",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            BUTTON_SHIFT_X(2), BUTTON_SHIFT_Y(3),
            g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
            hwnd,
            (HMENU)IDC_BUTTON_POINT,
            GetModuleHandle(NULL),
            NULL
        );

        CHAR operation[2] = "";
        for (int i = 0; i < 4; i++)
        {
            operation[0] = g_OPERATIONS[i];
            CreateWindowEx
            (
                NULL, "Button", operation,
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                BUTTON_SHIFT_X(3), BUTTON_SHIFT_Y(3 - i),
                g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
                hwnd,
                (HMENU)(IDC_BUTTON_PLUS + i),
                GetModuleHandle(NULL),
                NULL
            );
        }

        CreateWindowEx
        (
            NULL, "Button", "<--",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            BUTTON_SHIFT_X(4), BUTTON_SHIFT_Y(0),
            g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
            hwnd,
            (HMENU)IDC_BUTTON_BSP,
            GetModuleHandle(NULL),
            NULL
        );

        CreateWindowEx
        (
            NULL, "Button", "C",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            BUTTON_SHIFT_X(4), BUTTON_SHIFT_Y(1),
            g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
            hwnd,
            (HMENU)IDC_BUTTON_CLR,
            GetModuleHandle(NULL),
            NULL
        );

        CreateWindowEx
        (
            NULL, "Button", "=",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            BUTTON_SHIFT_X(4), BUTTON_SHIFT_Y(2),
            g_i_BUTTON_SIZE, g_i_BUTTON_DOUBLE_SIZE,
            hwnd,
            (HMENU)IDC_BUTTON_EQUAL,
            GetModuleHandle(NULL),
            NULL
        );
    }
    break;

    case WM_KEYDOWN:
    {
        CHAR sz_display[256];
        HWND hEdit = GetDlgItem(hwnd, IDC_EDIT);
        SendMessage(hEdit, WM_GETTEXT, sizeof(sz_display), (LPARAM)sz_display);

        // Обработка цифр
        if (wParam >= '0' && wParam <= '9')
        {
            CHAR sz_digit[2] = { (CHAR)wParam, '\0' };
            if (input_operation)
                sz_display[0] = 0;
            if (strcmp(sz_display, "0"))
                strcat(sz_display, sz_digit);
            else
                strcpy(sz_display, sz_digit);
            SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)sz_display);
            input = TRUE;
            input_operation = FALSE;
        }

        // Обработка точки
        if (wParam == VK_OEM_PERIOD)
        {
            if (strchr(sz_display, '.') == NULL)
            {
                strcat(sz_display, ".");
                SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)sz_display);
            }
        }

        // Обработка Backspace
        if (wParam == VK_BACK)
        {
            if (strlen(sz_display) == 1)
                sz_display[0] = '0';
            else
                sz_display[strlen(sz_display) - 1] = 0;
            SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)sz_display);
        }

        // Обработка знаков операций (+, -, *, /)
        if (wParam == VK_OEM_PLUS || wParam == VK_OEM_MINUS || wParam == VK_MULTIPLY || wParam == VK_DIVIDE)
        {
            if (input)
            {
                // Выполняем предыдущую операцию с результатом
                switch (operation)
                {
                case IDC_BUTTON_PLUS: result += current_num; break;
                case IDC_BUTTON_MINUS: result -= current_num; break;
                case IDC_BUTTON_ASTER: result *= current_num; break;
                case IDC_BUTTON_SLASH:
                    if (current_num != 0)
                        result /= current_num;
                    else
                        strcpy(sz_display, "Error");
                    break;
                }

                current_num = atof(sz_display); // Сохраняем текущее число
            }

            // Запоминаем операцию
            if (wParam == VK_OEM_PLUS)
                operation = IDC_BUTTON_PLUS;
            else if (wParam == VK_OEM_MINUS)
                operation = IDC_BUTTON_MINUS;
            else if (wParam == VK_MULTIPLY)
                operation = IDC_BUTTON_ASTER;
            else if (wParam == VK_DIVIDE)
                operation = IDC_BUTTON_SLASH;

            input_operation = TRUE;
            input = FALSE;
        }

        // Обработка знака "=" (клавиша Enter)
        if (wParam == VK_RETURN)
        {
            if (input)
                current_num = atof(sz_display); // Сохраняем последнее число

            // Выполняем операцию с последним значением
            switch (operation)
            {
            case IDC_BUTTON_PLUS: result += current_num; break;
            case IDC_BUTTON_MINUS: result -= current_num; break;
            case IDC_BUTTON_ASTER: result *= current_num; break;
            case IDC_BUTTON_SLASH:
                if (current_num != 0)
                    result /= current_num;
                else
                    strcpy(sz_display, "Error");
                break;
            }

            input_operation = FALSE;
            input = FALSE;

            // Округляем результат до 6 знаков после запятой
            if (result == (int)result)
                sprintf(sz_display, "%d", (int)result);  // если целое число
            else
                sprintf(sz_display, "%.6f", result);  // иначе 6 знаков после запятой

            SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)sz_display);
        }
    }
    break;

    case WM_COMMAND:
    {
        CONST INT SIZE = 256;
        CHAR sz_display[SIZE] = {};
        CHAR sz_digit[2] = {};
        HWND hEdit = GetDlgItem(hwnd, IDC_EDIT);
        SendMessage(hEdit, WM_GETTEXT, SIZE, (LPARAM)sz_display);

        // Обработка цифр
        if (LOWORD(wParam) >= IDC_BUTTON_0 && LOWORD(wParam) <= IDC_BUTTON_9)
        {
            if (input_operation)
                sz_display[0] = 0;
            sz_digit[0] = LOWORD(wParam) - IDC_BUTTON_0 + 48;
            if (strcmp(sz_display, "0"))
                strcat(sz_display, sz_digit);
            else
                strcpy(sz_display, sz_digit);
            SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)sz_display);
            input = TRUE;
            input_operation = FALSE;
        }

        // Обработка точки
        if (LOWORD(wParam) == IDC_BUTTON_POINT && strchr(sz_display, '.') == NULL)
        {
            strcat(sz_display, ".");
            SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)sz_display);
        }

        // Обработка Backspace
        if (LOWORD(wParam) == IDC_BUTTON_BSP)
        {
            if (strlen(sz_display) == 1)
                sz_display[0] = '0';
            else
                sz_display[strlen(sz_display) - 1] = 0;
            SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)sz_display);
        }

        // Обработка очистки
        if (LOWORD(wParam) == IDC_BUTTON_CLR)
        {
            result = 0;
            current_num = 0;
            operation = 0;
            input = input_operation = FALSE;
            SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)"0");
        }

        // Обработка знаков операций
        if (LOWORD(wParam) >= IDC_BUTTON_PLUS && LOWORD(wParam) <= IDC_BUTTON_SLASH)
        {
            if (input)
            {
                // Выполняем операцию с текущим числом
                switch (operation)
                {
                case IDC_BUTTON_PLUS: result += current_num; break;
                case IDC_BUTTON_MINUS: result -= current_num; break;
                case IDC_BUTTON_ASTER: result *= current_num; break;
                case IDC_BUTTON_SLASH:
                    if (current_num != 0)
                        result /= current_num;
                    else
                        strcpy(sz_display, "Error");
                    break;
                }
            }

            operation = LOWORD(wParam);  // Сохраняем операцию
            current_num = atof(sz_display);  // Сохраняем текущее число
            input = FALSE;
            input_operation = TRUE;
        }

        if (LOWORD(wParam) == IDC_BUTTON_EQUAL)
        {
            if (input) b = atof(sz_display);

            switch (operation)
            {
            case IDC_BUTTON_PLUS: result += current_num; break;
            case IDC_BUTTON_MINUS: result -= current_num; break;
            case IDC_BUTTON_ASTER: result *= current_num; break;
            case IDC_BUTTON_SLASH:
                if (current_num != 0)
                    result /= current_num;
                else
                    strcpy(sz_display, "Error");
                break;
            }

            input_operation = FALSE;
            input = FALSE;

            if (result == (int)result)
                sprintf(sz_display, "%d", (int)result);
            else
                sprintf(sz_display, "%.6f", result);

            SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)sz_display);
        }
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
    return FALSE;
}
