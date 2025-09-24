#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable: 4100 6386 28182 4244 4996)  // Игнор warnings
#include <Windows.h>
#include "resource.h"  // Ваш файл (убедитесь, что .rc включает "resource.h")
#include <string>

#define EM_SETCUEBANNER 0x1501

INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// UTF-8 строки для placeholder (сохраните в UTF-8 with BOM!)
const char* PROMPT_LOGIN_UTF8 = "Введите ваш логин";
const char* PROMPT_PASSWORD_UTF8 = "Введите пароль";

// Функция: UTF-8 -> UTF-16
std::wstring Utf8ToWide(const char* utf8String) {
    if (utf8String == nullptr) return L"";
    int wideSize = MultiByteToWideChar(CP_UTF8, 0, utf8String, -1, nullptr, 0);
    if (wideSize == 0) return L"";
    std::wstring wideBuffer(wideSize - 1, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8String, -1, &wideBuffer[0], wideSize);
    return wideBuffer;
}

INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    std::wstring wideLoginPrompt = Utf8ToWide(PROMPT_LOGIN_UTF8);
    std::wstring widePasswordPrompt = Utf8ToWide(PROMPT_PASSWORD_UTF8);
    if (wideLoginPrompt.empty() || widePasswordPrompt.empty()) {
        MessageBoxW(nullptr, L"Error: UTF-8 conversion failed! Save file as UTF-8 with BOM.", L"Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    // Передаем строки подсказок напрямую как lParam (или через структуру).
    // Поскольку мы не используем lParam для передачи, мы будем использовать статическую переменную или глобальную.
    // Для простоты, сделаем их статическими внутри DlgProc.

    INT_PTR result = DialogBoxParamW(hInstance, (LPCWSTR)MAKEINTRESOURCE(IDD_DIALOG1), nullptr, (DLGPROC)DlgProc, 0);
    return (int)result;
}

INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Теперь строки подсказок будут доступны через статические переменные внутри DlgProc
    static std::wstring wideLoginPrompt = Utf8ToWide(PROMPT_LOGIN_UTF8);
    static std::wstring widePasswordPrompt = Utf8ToWide(PROMPT_PASSWORD_UTF8);

    switch (uMsg) {
    case WM_INITDIALOG: {
        if (!wideLoginPrompt.empty()) {
            // Логин
            HWND hEditLogin = GetDlgItem(hwnd, IDC_EDIT_LOGIN);
            if (hEditLogin != nullptr) {
                // Всегда пытаемся установить EM_SETCUEBANNER, если он поддерживается.
                // Если не поддерживается (старая ОС), сообщение будет проигнорировано.
                SendMessageW(hEditLogin, EM_SETCUEBANNER, FALSE, (LPARAM)wideLoginPrompt.c_str());
            }
            else {
                MessageBoxW(hwnd, L"Error: IDC_EDIT_LOGIN not found! Fix .rc include to 'resource.h'.", L"Error", MB_OK | MB_ICONERROR);
            }

            // Пароль (аналогично)
            HWND hEditPassword = GetDlgItem(hwnd, IDC_EDIT_PASSWORD);
            if (hEditPassword != nullptr && !widePasswordPrompt.empty()) {
                SendMessageW(hEditPassword, EM_SETCUEBANNER, FALSE, (LPARAM)widePasswordPrompt.c_str());
            }
        }
        else {
            // Fallback, если конвертация UTF-8 провалилась
            HWND hEditLogin = GetDlgItem(hwnd, IDC_EDIT_LOGIN);
            if (hEditLogin != nullptr) SendMessageW(hEditLogin, EM_SETCUEBANNER, FALSE, (LPARAM)L"Enter login");
        }

        // Иконка (ваш .ico)
        HICON hIcon = LoadIconW(GetModuleHandleW(nullptr), (LPCWSTR)MAKEINTRESOURCE(IDI_ICON1));
        if (hIcon != nullptr) {
            SendMessageW(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
        }

        // Фокус на OK (placeholder виден)
        HWND hOKButton = GetDlgItem(hwnd, IDOK);
        if (hOKButton != nullptr) {
            SetFocus(hOKButton);
        }

        return TRUE;
    }

    case WM_COMMAND: {
        switch (LOWORD(wParam)) {
        case IDOK: {
            HWND hEditLogin = GetDlgItem(hwnd, IDC_EDIT_LOGIN);
            if (hEditLogin != nullptr) {
                wchar_t szLogin[256] = { 0 };
                GetWindowTextW(hEditLogin, szLogin, sizeof(szLogin) / sizeof(wchar_t));

                // Проверка логина (пусто или placeholder)
                bool loginEmpty = (wcslen(szLogin) == 0);
                // Сравниваем с оригинальной UTF-8 строкой, если она была успешно сконвертирована
                if (!wideLoginPrompt.empty() && wcscmp(szLogin, wideLoginPrompt.c_str()) == 0) {
                    loginEmpty = true;
                }

                if (loginEmpty) {
                    MessageBoxW(hwnd, L"Пожалуйста, введите ваш логин.", L"Ошибка", MB_OK | MB_ICONERROR);
                    return TRUE;
                }

                // Проверка пароля (аналогично)
                HWND hEditPassword = GetDlgItem(hwnd, IDC_EDIT_PASSWORD);
                wchar_t szPassword[256] = { 0 };
                if (hEditPassword != nullptr) {
                    GetWindowTextW(hEditPassword, szPassword, sizeof(szPassword) / sizeof(wchar_t));
                    bool passwordEmpty = (wcslen(szPassword) == 0);
                    // Сравниваем с оригинальной UTF-8 строкой, если она была успешно сконвертирована
                    if (!widePasswordPrompt.empty() && wcscmp(szPassword, widePasswordPrompt.c_str()) == 0) {
                        passwordEmpty = true;
                    }

                    if (passwordEmpty) {
                        MessageBoxW(hwnd, L"Пожалуйста, введите пароль.", L"Ошибка", MB_OK | MB_ICONERROR);
                        return TRUE;
                    }
                }

                // Успех
                MessageBoxW(hwnd, L"Вы успешно вошли!", L"Информация", MB_OK | MB_ICONINFORMATION);
                EndDialog(hwnd, 1);
            }
            break;
        }

        case IDCANCEL:
            EndDialog(hwnd, 0);
            break;

        case IDC_BUTTON_COPY: {
            HWND hEditLogin = GetDlgItem(hwnd, IDC_EDIT_LOGIN);
            if (hEditLogin != nullptr) {
                wchar_t szBuffer[256] = { 0 };
                GetWindowTextW(hEditLogin, szBuffer, sizeof(szBuffer) / sizeof(wchar_t));

                // Проверяем, не является ли введенный текст пустым или placeholder'ом
                bool isPlaceholder = false;
                if (!wideLoginPrompt.empty() && wcscmp(szBuffer, wideLoginPrompt.c_str()) == 0) {
                    isPlaceholder = true;
                }

                if (wcslen(szBuffer) > 0 && !isPlaceholder) {
                    if (OpenClipboard(hwnd)) {
                        EmptyClipboard();
                        size_t len = (wcslen(szBuffer) + 1) * sizeof(wchar_t);
                        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
                        if (hMem) {
                            wchar_t* pMem = (wchar_t*)GlobalLock(hMem);
                            wcscpy_s(pMem, len / sizeof(wchar_t), szBuffer);
                            GlobalUnlock(hMem);
                            SetClipboardData(CF_UNICODETEXT, hMem);
                        }
                        CloseClipboard();
                        MessageBoxW(hwnd, L"Логин скопирован в буфер обмена!", L"Копировано", MB_OK | MB_ICONINFORMATION);
                    }
                }
                else {
                    MessageBoxW(hwnd, L"Сначала введите логин.", L"Ошибка", MB_OK | MB_ICONWARNING);
                }
            }
            break;
        }
        }
        break;
    }

    case WM_CLOSE:
        EndDialog(hwnd, 0);
        break;
    }
    return FALSE;
}