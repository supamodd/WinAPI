#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <cstdio>
#include "resource.h"
#include <vector>
#include <string>

std::vector<std::string> listItems;  
CONST CHAR* VALUES[] = { "This", "is", "my", "First", "Combo", "Box" };  

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK AddEditDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
    DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DlgProc, 0);
    return 0;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
    {
        
        HWND hListBox = GetDlgItem(hwnd, IDC_LISTBOX);
        for (const auto& item : VALUES)
        {
            SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)item);
            listItems.push_back(item);  
        }
        HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
        SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    }
    break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON_ADD:  
        {
            DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ADD_EDIT_DIALOG), hwnd, (DLGPROC)AddEditDlgProc, (LPARAM)NULL);
        }
        break;

        case IDC_BUTTON_DELETE: 
        {
            HWND hListBox = GetDlgItem(hwnd, IDC_LISTBOX);
            int index = SendMessage(hListBox, LB_GETCURSEL, 0, 0); 
            if (index != LB_ERR)
            {
                SendMessage(hListBox, LB_DELETESTRING, index, 0);  
                listItems.erase(listItems.begin() + index); 
            }
        }
        break;

        case IDC_LISTBOX: 
            if (HIWORD(wParam) == LBN_DBLCLK)
            {
                HWND hListBox = GetDlgItem(hwnd, IDC_LISTBOX);
                int index = SendMessage(hListBox, LB_GETCURSEL, 0, 0); 
                if (index != LB_ERR)
                {
                    const int bufferSize = 256;
                    CHAR szItem[bufferSize] = {};
                    SendMessage(hListBox, LB_GETTEXT, index, (LPARAM)szItem); 

                   
                    DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ADD_EDIT_DIALOG), hwnd, (DLGPROC)AddEditDlgProc, (LPARAM)szItem);
                }
            }
            break;

        case IDCANCEL:
            EndDialog(hwnd, 0);
            break;
        }
        break;

    case WM_CLOSE:
        EndDialog(hwnd, 0);
        break;
    }
    return FALSE;
}

BOOL CALLBACK AddEditDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static char itemName[256]; 

    switch (uMsg)
    {
    case WM_INITDIALOG:
    {
       
        if (lParam != NULL)
        {
            strncpy(itemName, (char*)lParam, sizeof(itemName) - 1);
            SetDlgItemText(hwnd, IDC_EDIT_ITEM, itemName);
        }
    }
    break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK: 
        {
            GetDlgItemText(hwnd, IDC_EDIT_ITEM, itemName, sizeof(itemName) - 1);
            if (strlen(itemName) > 0)
            {
               
                HWND hListBox = GetDlgItem(GetParent(hwnd), IDC_LISTBOX);
                int index = SendMessage(hListBox, LB_GETCURSEL, 0, 0);
                if (index == LB_ERR)  
                {
                    SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)itemName);
                    listItems.push_back(itemName); 
                }
                else  
                {
                    SendMessage(hListBox, LB_DELETESTRING, index, 0); 
                    listItems.erase(listItems.begin() + index);  

                    SendMessage(hListBox, LB_INSERTSTRING, index, (LPARAM)itemName); 
                    listItems.insert(listItems.begin() + index, itemName);  
                }
            }
            EndDialog(hwnd, 0);
        }
        break;

        case IDCANCEL:
            EndDialog(hwnd, 0);
            break;
        }
        break;
    }
    return FALSE;
}
