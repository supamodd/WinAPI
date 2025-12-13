#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <float.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <gdiplus.h>
#include <Shlwapi.h> 
#include "resource.h"
#include "reason.h"

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "Shlwapi.lib") 

CONST CHAR g_sz_CLASS_NAME[] = "Calc_SPU_411";

CONST INT g_i_BUTTON_SIZE = 80;
CONST INT g_i_INTERVAL = 2;
CONST INT g_i_BUTTON_DOUBLE_SIZE = g_i_BUTTON_SIZE * 2 + g_i_INTERVAL;

CONST INT g_i_START_X = 10;
CONST INT g_i_START_Y = 10;

CONST INT g_i_SCREEN_WIDTH = (g_i_BUTTON_SIZE + g_i_INTERVAL) * 5 - g_i_INTERVAL;
CONST INT g_i_SCREEN_HEIGHT = g_i_BUTTON_SIZE;

CONST INT g_i_BUTTON_START_X = g_i_START_X;
CONST INT g_i_BUTTON_START_Y = g_i_START_Y + g_i_SCREEN_HEIGHT + g_i_INTERVAL * 4;

CONST CHAR g_OPERATIONS[] = "+-*/";

#define BUTTON_SHIFT_X(shift) g_i_BUTTON_START_X + (g_i_BUTTON_SIZE + g_i_INTERVAL)*(shift)
#define BUTTON_SHIFT_Y(shift) g_i_BUTTON_START_Y + (g_i_BUTTON_SIZE + g_i_INTERVAL)*(shift)

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
VOID SetSkin(HWND hwnd, CONST CHAR SZ_SKIN[]);

static HMODULE g_hCurrentSkin = NULL; 

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    WNDCLASSEX wClass = { 0 };
    wClass.cbSize = sizeof(wClass);
    wClass.lpfnWndProc = WndProc;
    wClass.hInstance = hInstance;
    wClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
    wClass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
    wClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wClass.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wClass.lpszClassName = g_sz_CLASS_NAME;

    if (!RegisterClassEx(&wClass))
    {
        MessageBox(NULL, "Class registration failed", "Error", MB_OK | MB_ICONERROR);
        Gdiplus::GdiplusShutdown(gdiplusToken);
        return 0;
    }

    HWND hwnd = CreateWindowEx(
        0, g_sz_CLASS_NAME, g_sz_CLASS_NAME,
        WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        g_i_SCREEN_WIDTH + 2 * g_i_START_X + 16,
        g_i_BUTTON_START_Y + (g_i_BUTTON_SIZE + g_i_INTERVAL) * 4 + 42,
        NULL, NULL, hInstance, NULL
    );

    if (!hwnd)
    {
        MessageBox(NULL, "Failed to create window", "Error", MB_OK | MB_ICONERROR);
        Gdiplus::GdiplusShutdown(gdiplusToken);
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

    Gdiplus::GdiplusShutdown(gdiplusToken);
    return (INT)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static DOUBLE a = DBL_MIN;
    static DOUBLE b = DBL_MIN;
    static INT operation = 0;
    static BOOL input = FALSE;
    static BOOL input_operation = FALSE;
    static HMENU hMenu = NULL;
    static HFONT hFont = NULL;
    static Gdiplus::PrivateFontCollection* g_pFontCollection = nullptr;
    static HGLOBAL g_hFontMem = nullptr;
    static HBITMAP g_hButtons[18] = { 0 };
    // static HMODULE g_hCurrentSkin = NULL;

    switch (uMsg)
    {
    case WM_CREATE:
    {
        HWND hEdit = CreateWindowEx(
            0, "Edit", "0",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT,
            g_i_START_X, g_i_START_Y,
            g_i_SCREEN_WIDTH, g_i_SCREEN_HEIGHT,
            hwnd, (HMENU)IDC_EDIT, GetModuleHandle(NULL), NULL
        );

        INT digit = 1;
        CHAR sz_digit[2] = "";
        for (int i = 6; i >= 0; i -= 3)
        {
            for (int j = 0; j < 3; j++)
            {
                sz_digit[0] = digit + '0';
                CreateWindowEx(
                    0, "Button", sz_digit,
                    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
                    g_i_BUTTON_START_X + (g_i_BUTTON_SIZE + g_i_INTERVAL) * j,
                    g_i_BUTTON_START_Y + (g_i_BUTTON_SIZE + g_i_INTERVAL) * (i / 3),
                    g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
                    hwnd, (HMENU)(IDC_BUTTON_0 + digit), GetModuleHandle(NULL), NULL
                );
                digit++;
            }
        }

        CreateWindowEx(0, "Button", "0", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
            g_i_BUTTON_START_X, BUTTON_SHIFT_Y(3), g_i_BUTTON_DOUBLE_SIZE, g_i_BUTTON_SIZE,
            hwnd, (HMENU)IDC_BUTTON_0, GetModuleHandle(NULL), NULL);

        CreateWindowEx(0, "Button", ".", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
            BUTTON_SHIFT_X(2), BUTTON_SHIFT_Y(3), g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
            hwnd, (HMENU)IDC_BUTTON_POINT, GetModuleHandle(NULL), NULL);

        CHAR op[2] = "";
        for (int i = 0; i < 4; i++)
        {
            op[0] = g_OPERATIONS[i];
            CreateWindowEx(0, "Button", op, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
                BUTTON_SHIFT_X(3), BUTTON_SHIFT_Y(3 - i), g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
                hwnd, (HMENU)(IDC_BUTTON_PLUS + i), GetModuleHandle(NULL), NULL);
        }

        CreateWindowEx(0, "Button", "<-", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
            BUTTON_SHIFT_X(4), BUTTON_SHIFT_Y(0), g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
            hwnd, (HMENU)IDC_BUTTON_BSP, GetModuleHandle(NULL), NULL);

        CreateWindowEx(0, "Button", "C", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
            BUTTON_SHIFT_X(4), BUTTON_SHIFT_Y(1), g_i_BUTTON_SIZE, g_i_BUTTON_SIZE,
            hwnd, (HMENU)IDC_BUTTON_CLR, GetModuleHandle(NULL), NULL);

        CreateWindowEx(0, "Button", "=", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
            BUTTON_SHIFT_X(4), BUTTON_SHIFT_Y(2), g_i_BUTTON_SIZE, g_i_BUTTON_DOUBLE_SIZE,
            hwnd, (HMENU)IDC_BUTTON_EQUAL, GetModuleHandle(NULL), NULL);

        hMenu = CreatePopupMenu();
        AppendMenu(hMenu, MF_STRING, 1, "square_blue");
        AppendMenu(hMenu, MF_STRING, 2, "metal_mistral");
        AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
        AppendMenu(hMenu, MF_STRING, 3, "Dikoe Disco");
        AppendMenu(hMenu, MF_STRING, 4, "StarF");
        AppendMenu(hMenu, MF_STRING, 5, "Steiglitz");

        SetSkin(hwnd, "metal_mistral");
        hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
        SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

        g_pFontCollection = new Gdiplus::PrivateFontCollection();
        g_hFontMem = nullptr;
    }
    break;

    case WM_CONTEXTMENU:
    {
        if (hMenu)
        {
            POINT pt;
            GetCursorPos(&pt);
            TrackPopupMenu(hMenu, TPM_RIGHTALIGN | TPM_BOTTOMALIGN, pt.x, pt.y, 0, hwnd, NULL);
        }
    }
    break;

    case WM_COMMAND:
    {
        if (LOWORD(wParam) == 1) SetSkin(hwnd, "square_blue");
        else if (LOWORD(wParam) == 2) SetSkin(hwnd, "metal_mistral");

        else if (LOWORD(wParam) >= 3 && LOWORD(wParam) <= 5)
        {
            HWND hEdit = GetDlgItem(hwnd, IDC_EDIT);
            if (hFont) { DeleteObject(hFont); hFont = nullptr; }
            if (g_hFontMem) { GlobalFree(g_hFontMem); g_hFontMem = nullptr; }

            const CHAR* szFontFile = nullptr;
            switch (LOWORD(wParam))
            {
            case 3: szFontFile = "Fonts\\Dikoe_disco.ttf"; break;
            case 4: szFontFile = "Fonts\\StarF.ttf";       break;
            case 5: szFontFile = "Fonts\\Steiglitz.ttf";   break;
            }

            if (!szFontFile || GetFileAttributes(szFontFile) == INVALID_FILE_ATTRIBUTES)
            {
                MessageBox(hwnd, "Font file not found!", "Error", MB_OK | MB_ICONERROR);
                goto apply_default;
            }

            {
                HANDLE hFile = CreateFile(szFontFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
                if (hFile == INVALID_HANDLE_VALUE) goto apply_default;

                DWORD dwSize = GetFileSize(hFile, NULL);
                g_hFontMem = GlobalAlloc(GMEM_MOVEABLE, dwSize);
                if (!g_hFontMem) { CloseHandle(hFile); goto apply_default; }

                BYTE* pData = (BYTE*)GlobalLock(g_hFontMem);
                DWORD dwRead;
                ReadFile(hFile, pData, dwSize, &dwRead, NULL);
                GlobalUnlock(g_hFontMem);
                CloseHandle(hFile);

                if (g_pFontCollection)
                {
                    g_pFontCollection->~PrivateFontCollection();
                    delete g_pFontCollection;
                }
                g_pFontCollection = new Gdiplus::PrivateFontCollection();

                if (g_pFontCollection->AddMemoryFont(pData, dwSize) != Gdiplus::Ok)
                {
                    GlobalFree(g_hFontMem); g_hFontMem = nullptr;
                    goto apply_default;
                }

                int familyCount = g_pFontCollection->GetFamilyCount();
                if (familyCount > 0)
                {
                    Gdiplus::FontFamily* family = new Gdiplus::FontFamily();
                    g_pFontCollection->GetFamilies(1, family, &familyCount);

                    WCHAR wFamilyName[LF_FACESIZE] = { 0 };
                    family->GetFamilyName(wFamilyName, 0);

                    LOGFONTA lf = { 0 };
                    lf.lfHeight = -MulDiv(24, GetDeviceCaps(GetDC(hEdit), LOGPIXELSY), 72);
                    lf.lfWeight = FW_NORMAL;
                    lf.lfOutPrecision = OUT_TT_PRECIS;
                    lf.lfQuality = ANTIALIASED_QUALITY;
                    lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;

                    WideCharToMultiByte(CP_UTF8, 0, wFamilyName, -1, lf.lfFaceName, LF_FACESIZE, NULL, NULL);

                    hFont = CreateFontIndirectA(&lf); 
                    if (hFont)
                    {
                        SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
                        InvalidateRect(hEdit, NULL, TRUE);
                    }

                    delete family;
                }
            }
            break;

        apply_default:
            hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
            SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
        }

        else
        {
            CONST INT SIZE = 256;
            CHAR sz_display[SIZE] = {};
            CHAR sz_digit[2] = {};
            HWND hEdit = GetDlgItem(hwnd, IDC_EDIT);
            SendMessage(hEdit, WM_GETTEXT, SIZE, (LPARAM)sz_display);

            if (LOWORD(wParam) >= IDC_BUTTON_0 && LOWORD(wParam) <= IDC_BUTTON_9)
            {
                if (input_operation) sz_display[0] = 0;
                sz_digit[0] = LOWORD(wParam) - IDC_BUTTON_0 + '0';
                if (strcmp(sz_display, "0")) strcat(sz_display, sz_digit);
                else strcpy(sz_display, sz_digit);
                SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)sz_display);
                input = TRUE;
                input_operation = FALSE;
            }

            if (LOWORD(wParam) == IDC_BUTTON_POINT && strchr(sz_display, '.') == NULL)
            {
                strcat(sz_display, ".");
                SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)sz_display);
            }

            if (LOWORD(wParam) == IDC_BUTTON_BSP)
            {
                size_t len = strlen(sz_display);
                if (len <= 1) strcpy(sz_display, "0");
                else sz_display[len - 1] = 0;
                SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)sz_display);
            }

            if (LOWORD(wParam) == IDC_BUTTON_CLR)
            {
                a = b = DBL_MIN;
                operation = 0;
                input = input_operation = FALSE;
                SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)"0");
            }

            if (LOWORD(wParam) >= IDC_BUTTON_PLUS && LOWORD(wParam) <= IDC_BUTTON_SLASH)
            {
                if (input)
                {
                    (a == DBL_MIN ? a : b) = atof(sz_display);
                    SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_EQUAL, 0);
                }
                input = FALSE;
                operation = LOWORD(wParam);
                input_operation = TRUE;
            }

            if (LOWORD(wParam) == IDC_BUTTON_EQUAL)
            {
                if (input) b = atof(sz_display);
                switch (operation)
                {
                case IDC_BUTTON_PLUS:  a += b; break;
                case IDC_BUTTON_MINUS: a -= b; break;
                case IDC_BUTTON_ASTER: a *= b; break;
                case IDC_BUTTON_SLASH: a = (b != 0) ? a / b : 0; break;
                }
                input = input_operation = FALSE;
                sprintf(sz_display, fabs(floor(a + 0.5) - a) < 1e-9 ? "%.0f" : "%g", a);
                SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)sz_display);
            }
        }
    }
    break;

    case WM_KEYDOWN:
    {
        if (GetKeyState(VK_SHIFT) < 0)
        {
            if (wParam == '8')
                SendMessage(GetDlgItem(hwnd, IDC_BUTTON_ASTER), BM_SETSTATE, TRUE, 0);
            if (wParam == VK_OEM_PLUS)
                SendMessage(GetDlgItem(hwnd, IDC_BUTTON_PLUS), BM_SETSTATE, TRUE, 0);
        }
        else if (wParam >= '0' && wParam <= '9')
        {
            SendMessage(GetDlgItem(hwnd, wParam - '0' + IDC_BUTTON_0), BM_SETSTATE, TRUE, 0);
        }
        if (wParam >= 0x60 && wParam <= 0x69)
        {
            SendMessage(GetDlgItem(hwnd, wParam - 0x60 + IDC_BUTTON_0), BM_SETSTATE, TRUE, 0);
        }
        switch (wParam)
        {
        case VK_ADD:      SendMessage(GetDlgItem(hwnd, IDC_BUTTON_PLUS), BM_SETSTATE, TRUE, 0); break;
        case VK_SUBTRACT:
        case VK_OEM_MINUS:SendMessage(GetDlgItem(hwnd, IDC_BUTTON_MINUS), BM_SETSTATE, TRUE, 0); break;
        case VK_MULTIPLY: SendMessage(GetDlgItem(hwnd, IDC_BUTTON_ASTER), BM_SETSTATE, TRUE, 0); break;
        case VK_DIVIDE:
        case VK_OEM_2:    SendMessage(GetDlgItem(hwnd, IDC_BUTTON_SLASH), BM_SETSTATE, TRUE, 0); break;
        case VK_DECIMAL:
        case VK_OEM_PERIOD: SendMessage(GetDlgItem(hwnd, IDC_BUTTON_POINT), BM_SETSTATE, TRUE, 0); break;
        case VK_BACK:     SendMessage(GetDlgItem(hwnd, IDC_BUTTON_BSP), BM_SETSTATE, TRUE, 0); break;
        case VK_ESCAPE:   SendMessage(GetDlgItem(hwnd, IDC_BUTTON_CLR), BM_SETSTATE, TRUE, 0); break;
        case VK_RETURN:   SendMessage(GetDlgItem(hwnd, IDC_BUTTON_EQUAL), BM_SETSTATE, TRUE, 0); break;
        }
    }
    break;

    case WM_KEYUP:
    {
        if (GetKeyState(VK_SHIFT) < 0)
        {
            if (wParam == '8')
            {
                SendMessage(GetDlgItem(hwnd, IDC_BUTTON_ASTER), BM_SETSTATE, FALSE, 0);
                SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_ASTER, 0);
            }
            if (wParam == VK_OEM_PLUS)
            {
                SendMessage(GetDlgItem(hwnd, IDC_BUTTON_PLUS), BM_SETSTATE, FALSE, 0);
                SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_PLUS, 0);
            }
        }
        else if (wParam >= '0' && wParam <= '9')
        {
            SendMessage(GetDlgItem(hwnd, wParam - '0' + IDC_BUTTON_0), BM_SETSTATE, FALSE, 0);
            SendMessage(hwnd, WM_COMMAND, wParam - '0' + IDC_BUTTON_0, 0);
        }
        if (wParam >= 0x60 && wParam <= 0x69)
        {
            SendMessage(GetDlgItem(hwnd, wParam - 0x60 + IDC_BUTTON_0), BM_SETSTATE, FALSE, 0);
            SendMessage(hwnd, WM_COMMAND, wParam - 0x60 + IDC_BUTTON_0, 0);
        }
        switch (wParam)
        {
        case VK_ADD:      SendMessage(GetDlgItem(hwnd, IDC_BUTTON_PLUS), BM_SETSTATE, FALSE, 0); SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_PLUS, 0);   break;
        case VK_SUBTRACT:
        case VK_OEM_MINUS:SendMessage(GetDlgItem(hwnd, IDC_BUTTON_MINUS), BM_SETSTATE, FALSE, 0); SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_MINUS, 0);  break;
        case VK_MULTIPLY: SendMessage(GetDlgItem(hwnd, IDC_BUTTON_ASTER), BM_SETSTATE, FALSE, 0); SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_ASTER, 0);  break;
        case VK_DIVIDE:
        case VK_OEM_2:    SendMessage(GetDlgItem(hwnd, IDC_BUTTON_SLASH), BM_SETSTATE, FALSE, 0); SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_SLASH, 0);  break;
        case VK_DECIMAL:
        case VK_OEM_PERIOD: SendMessage(GetDlgItem(hwnd, IDC_BUTTON_POINT), BM_SETSTATE, FALSE, 0); SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_POINT, 0); break;
        case VK_BACK:     SendMessage(GetDlgItem(hwnd, IDC_BUTTON_BSP), BM_SETSTATE, FALSE, 0); SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_BSP, 0);    break;
        case VK_ESCAPE:   SendMessage(GetDlgItem(hwnd, IDC_BUTTON_CLR), BM_SETSTATE, FALSE, 0); SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_CLR, 0);    break;
        case VK_RETURN:   SendMessage(GetDlgItem(hwnd, IDC_BUTTON_EQUAL), BM_SETSTATE, FALSE, 0); SendMessage(hwnd, WM_COMMAND, IDC_BUTTON_EQUAL, 0);  break;
        }
    }
    break;

    case WM_DESTROY:
        if (g_hCurrentSkin) { FreeLibrary(g_hCurrentSkin); g_hCurrentSkin = NULL; } 
        if (hMenu) DestroyMenu(hMenu);
        if (hFont) DeleteObject(hFont);
        if (g_pFontCollection)
        {
            delete g_pFontCollection;
            g_pFontCollection = nullptr;
        }
        if (g_hFontMem) GlobalFree(g_hFontMem);
        PostQuitMessage(0);
        break;

    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

VOID SetSkin(HWND hwnd, CONST CHAR SZ_SKIN[])
{
    if (g_hCurrentSkin) {
        FreeLibrary(g_hCurrentSkin);
        g_hCurrentSkin = NULL;
    }

    CHAR szDllPath[MAX_PATH];
    GetModuleFileNameA(NULL, szDllPath, MAX_PATH);
    PathRemoveFileSpecA(szDllPath);
    strcat(szDllPath, "\\");
    strcat(szDllPath, SZ_SKIN);
    strcat(szDllPath, ".dll");

    g_hCurrentSkin = LoadLibraryA(szDllPath);
    if (!g_hCurrentSkin)
    {
        CHAR msg[512];
        sprintf(msg, "Не удалось загрузить:\n%s", szDllPath);
        MessageBoxA(hwnd, msg, "Ошибка", MB_OK | MB_ICONERROR);
        return;
    }

    auto LoadBtn = [&](INT id, LPCSTR name, INT w, INT h) {
        HBITMAP hBmp = (HBITMAP)LoadImageA(g_hCurrentSkin, name, IMAGE_BITMAP, w, h, LR_CREATEDIBSECTION);
        if (hBmp) 
        {
            SendMessage(GetDlgItem(hwnd, id), BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp);
        }
        };

    for (int i = 0; i < 10; ++i) 
    {
        CHAR res[32];
        sprintf(res, "button_%d.bmp", i);
        LoadBtn(IDC_BUTTON_0 + i, res, i == 0 ? g_i_BUTTON_DOUBLE_SIZE : g_i_BUTTON_SIZE, g_i_BUTTON_SIZE);
    }

    LoadBtn(IDC_BUTTON_POINT, "button_point.bmp", g_i_BUTTON_SIZE, g_i_BUTTON_SIZE);

    const CHAR* ops[] = { "plus", "minus", "aster", "slash" };
    const INT op_ids[] = { IDC_BUTTON_PLUS, IDC_BUTTON_MINUS, IDC_BUTTON_ASTER, IDC_BUTTON_SLASH };
    for (int i = 0; i < 4; ++i) {
        CHAR res[32];
        sprintf(res, "button_%s.bmp", ops[i]);
        LoadBtn(op_ids[i], res, g_i_BUTTON_SIZE, g_i_BUTTON_SIZE);
    }

    LoadBtn(IDC_BUTTON_BSP, "button_bsp.bmp", g_i_BUTTON_SIZE, g_i_BUTTON_SIZE);
    LoadBtn(IDC_BUTTON_CLR, "button_clr.bmp", g_i_BUTTON_SIZE, g_i_BUTTON_SIZE);
    LoadBtn(IDC_BUTTON_EQUAL, "button_equal.bmp", g_i_BUTTON_SIZE, g_i_BUTTON_DOUBLE_SIZE);
}