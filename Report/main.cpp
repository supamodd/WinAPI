#include <windows.h>
#include <commctrl.h>
#include <string>
#include "resource.h"


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void generateReview(HWND hwnd);

// Глобальные переменные для доступа к элементам управления
HWND hwndNameEdit, hwndHardworkingCheck, hwndResponsibleCheck, hwndPunctualCheck, hwndDisorganizedCheck, hwndResultEdit;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "WindowClass";

    // Рег клас окна
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;      
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

    if (!RegisterClass(&wc)) 
    {
        return 0;
    }

    // Создаем окно
    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "Генератор отзыва",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 800,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (hwnd == nullptr) 
    {
        return 0;
    }

    // Создаем элементы управления
    hwndNameEdit = CreateWindowEx(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
        10, 30, 250, 20, hwnd, (HMENU)IDC_EDIT1, hInstance, nullptr);
    CreateWindowEx(0, "STATIC", "Введите Ф.И.О. студента:", WS_CHILD | WS_VISIBLE,
        10, 10, 250, 20, hwnd, nullptr, hInstance, nullptr);

    hwndHardworkingCheck = CreateWindowEx(0, "BUTTON", "Трудолюбивый", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
        10, 60, 500, 20, hwnd, (HMENU)IDC_CHECK1, hInstance, nullptr);
    hwndResponsibleCheck = CreateWindowEx(0, "BUTTON", "Ответственный", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
        10, 90, 500, 20, hwnd, (HMENU)IDC_CHECK2, hInstance, nullptr);
    hwndPunctualCheck = CreateWindowEx(0, "BUTTON", "Пунктуальный", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
        10, 120, 500, 20, hwnd, (HMENU)IDC_CHECK3, hInstance, nullptr);
    hwndDisorganizedCheck = CreateWindowEx(0, "BUTTON", "Неорганизованный", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
        10, 150, 500, 20, hwnd, (HMENU)IDC_CHECK4, hInstance, nullptr);

    hwndResultEdit = CreateWindowEx(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_READONLY | WS_VSCROLL,
        10, 180, 500, 60, hwnd, (HMENU)IDC_EDIT2, hInstance, nullptr);

    CreateWindowEx(0, "BUTTON", "Сгенерировать отзыв", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        100, 250, 500, 30, hwnd, (HMENU)IDC_BUTTON1, hInstance, nullptr);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Основной цикл сообщений
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

// Оконная процедура
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
    switch (uMsg) 
    {
    case WM_COMMAND: 
    {
        if (LOWORD(wParam) == IDC_BUTTON1)
        {
            generateReview(hwnd);
        }
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

// Функция для генерации отзыва
void generateReview(HWND hwnd)
{
    char name[100];
    GetWindowText(hwndNameEdit, name, sizeof(name));

    // Чтение состояний чекбоксов
    bool hardworking = (SendMessage(hwndHardworkingCheck, BM_GETCHECK, 0, 0) == BST_CHECKED);
    bool responsible = (SendMessage(hwndResponsibleCheck, BM_GETCHECK, 0, 0) == BST_CHECKED);
    bool punctual = (SendMessage(hwndPunctualCheck, BM_GETCHECK, 0, 0) == BST_CHECKED);
    bool disorganized = (SendMessage(hwndDisorganizedCheck, BM_GETCHECK, 0, 0) == BST_CHECKED);

    // Строим отзыв
    std::string result = "Отзыв о студенте: " + std::string(name) + "\n";

    if (hardworking) result += "- Трудолюбивый\n";
    if (responsible) result += "- Ответственный\n";
    if (punctual) result += "- Пунктуальный\n";
    if (disorganized) result += "- Неорганизованный\n";

    // Выводим результат в текстовое поле
    SetWindowText(hwndResultEdit, result.c_str());
}
