#include <Windows.h>
#include "resource.h"
#include <wchar.h>

CONST WCHAR g_sz_WND_CLASS_NAME[] = L"My Windows Class"; 

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
    WNDCLASSEX wClass;
    ZeroMemory(&wClass, sizeof(wClass));

    wClass.style = 0;
    wClass.cbSize = sizeof(wClass);
    wClass.cbWndExtra = 0;
    wClass.cbClsExtra = 0;

    wClass.hIconSm = (HICON)LoadImage(hInstance, L"window2win.ico", IMAGE_ICON, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_LOADFROMFILE);
    wClass.hIcon = (HICON)LoadImage(hInstance, L"desktop2win.ico", IMAGE_ICON, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_LOADFROMFILE);
    wClass.hCursor = (HCURSOR)LoadImage(hInstance, L"Working In Background.ani", IMAGE_CURSOR, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_LOADFROMFILE);
    wClass.hbrBackground = (HBRUSH)COLOR_WINDOW;

    wClass.hInstance = hInstance;
    wClass.lpszClassName = g_sz_WND_CLASS_NAME;
    wClass.lpszMenuName = NULL;
    wClass.lpfnWndProc = WndProc;

    if (!RegisterClassEx(&wClass))
    {
        MessageBox(NULL, L"Class registration failed", L"Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // размер окна
    int windowWidth = (screenWidth * 75) / 100;
    int windowHeight = (screenHeight * 75) / 100;

    // расположение экранаа
    int posX = (screenWidth - windowWidth) / 2;
    int posY = (screenHeight - windowHeight) / 2;

    
    HWND hwnd = CreateWindowEx
    (
        NULL, 
        g_sz_WND_CLASS_NAME, 
        g_sz_WND_CLASS_NAME, 
        WS_OVERLAPPEDWINDOW,
        posX, posY, 
        windowWidth, windowHeight, 
        NULL,
        NULL,
        hInstance,
        NULL
    );
    if (hwnd == NULL)
    {
        MessageBox(NULL, L"Window creation failed", L"Error", MB_OK | MB_ICONERROR); 
        return 0;
    }
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

   
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        HWND hStatic = CreateWindowEx
        (
            NULL,
            L"Static", 
            L"Text created with CreateWindowEx();", 
            WS_CHILD | WS_VISIBLE,
            10, 10,
            500, 25,
            hwnd,
            (HMENU)1000,
            GetModuleHandle(NULL),
            NULL
        );

        HWND hEdit = CreateWindowEx
        (
            NULL,
            L"Edit",
            L"", 
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            10, 45,
            500, 22,
            hwnd,
            (HMENU)1001,
            GetModuleHandle(NULL),
            NULL
        );

        HWND hButton = CreateWindowEx
        (
            NULL,
            L"Button", 
            L"Click me", 
            WS_CHILD | WS_VISIBLE,
            430, 70,
            80, 32,
            hwnd,
            (HMENU)1002,
            GetModuleHandle(NULL),
            NULL
        );
    }
    break;

    case WM_SIZE:
    {
       
        RECT rect;
        GetWindowRect(hwnd, &rect);

        
        wchar_t buffer[100];
        swprintf(buffer, 100, L"Width: %d, Height: %d, X: %d, Y: %d",
            rect.right - rect.left, rect.bottom - rect.top,
            rect.left, rect.top);

       
        SetWindowText(hwnd, buffer);
    }
    break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case 1002:
        {
            MessageBox(hwnd, L"Button clicked", L"Information", MB_OK | MB_ICONINFORMATION); 
            CONST INT SIZE = 256;
            CHAR sz_buffer[SIZE] = {};
            HWND hStatic = GetDlgItem(hwnd, 1000);
            HWND hEdit = GetDlgItem(hwnd, 1001);

            SendMessage(hEdit, WM_GETTEXT, SIZE, (LPARAM)sz_buffer);
            SendMessage(hStatic, WM_SETTEXT, 0, (LPARAM)sz_buffer);
            SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)sz_buffer);
            SendMessage(GetDlgItem(hwnd, 1002), WM_SETTEXT, 0, (LPARAM)sz_buffer);
        }
        break;
        }
        break;

    case WM_DESTROY:
        MessageBox(NULL, L"Application finished", L"Finita la comedia", MB_OK | MB_ICONERROR); 
        PostQuitMessage(0);
        break;

    case WM_CLOSE:
        if (MessageBox(hwnd, L"Are you sure you want to close?", L"Exit", MB_YESNO | MB_ICONQUESTION) == IDYES) 
            SendMessage(hwnd, WM_DESTROY, 0, 0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return FALSE;
}
