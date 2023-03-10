/*-----------------------------------------------------
   POPPAD2.C -- Popup Editor Version 2 (includes menu)
            (c) Charles Petzold, 1998
  -----------------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "Resource.h"

#define ID_EDIT     1

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

PCWSTR szAppName = L"PopPad2";

int WINAPI wWinMain(_In_     HINSTANCE instance,
                    _In_opt_ HINSTANCE prevInstance,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInstance);
   UNREFERENCED_PARAMETER(cmdLine);

   HACCEL    accel;
   HWND      hwnd;
   MSG       msg;
   WNDCLASSW wc;

   wc.style         = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = instance;
   wc.hIcon         = (HICON)   LoadImageW(instance, szAppName, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wc.hbrBackground = (HBRUSH)  (COLOR_WINDOW + 1);
   wc.lpszMenuName  = szAppName;
   wc.lpszClassName = szAppName;

   if ( !RegisterClassW(&wc) )
   {
      MessageBoxW(NULL, L"This program requires Windows NT!",
                  szAppName, MB_ICONERROR);
      return 0;
   }

   hwnd = CreateWindowW(szAppName, szAppName,
                        WS_OVERLAPPEDWINDOW,
                        GetSystemMetrics(SM_CXSCREEN) / 4,
                        GetSystemMetrics(SM_CYSCREEN) / 4,
                        GetSystemMetrics(SM_CXSCREEN) / 2,
                        GetSystemMetrics(SM_CYSCREEN) / 2,
                        NULL, NULL, instance, NULL);

   ShowWindow(hwnd, showCmd);
   UpdateWindow(hwnd);

   accel = LoadAcceleratorsW(instance, szAppName);

   while ( GetMessageW(&msg, NULL, 0, 0) )
   {
      if ( !TranslateAcceleratorW(hwnd, accel, &msg) )
      {
         TranslateMessage(&msg);
         DispatchMessageW(&msg);
      }
   }
   return (int) msg.wParam;
}

AskConfirmation(HWND hwnd)
{
   return MessageBoxW(hwnd, L"Really want to close PopPad2?",
                      szAppName, MB_YESNO | MB_ICONQUESTION);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   static HWND edit;
   int         select;
   int         iEnable;

   switch ( message )
   {
   case WM_CREATE:
      edit = CreateWindowW(L"edit", NULL,
                           WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
                           WS_BORDER | ES_LEFT | ES_MULTILINE |
                           ES_AUTOHSCROLL | ES_AUTOVSCROLL,
                           0, 0, 0, 0, hwnd, (HMENU) ID_EDIT,
                           ((LPCREATESTRUCT) lParam)->hInstance, NULL);
      return 0;

   case WM_SETFOCUS:
      SetFocus(edit);
      return 0;

   case WM_SIZE:
      MoveWindow(edit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
      return 0;

   case WM_INITMENUPOPUP:
      if ( lParam == 1 )
      {
         EnableMenuItem((HMENU) wParam, IDM_EDIT_UNDO,
                        SendMessageW(edit, EM_CANUNDO, 0, 0) ?
                        MF_ENABLED : MF_GRAYED);

         EnableMenuItem((HMENU) wParam, IDM_EDIT_PASTE,
                        IsClipboardFormatAvailable(CF_TEXT) ?
                        MF_ENABLED : MF_GRAYED);

         select = (int) SendMessage(edit, EM_GETSEL, 0, 0);

         if ( HIWORD(select) == LOWORD(select) )
            iEnable = MF_GRAYED;
         else
            iEnable = MF_ENABLED;

         EnableMenuItem((HMENU) wParam, IDM_EDIT_CUT, iEnable);
         EnableMenuItem((HMENU) wParam, IDM_EDIT_COPY, iEnable);
         EnableMenuItem((HMENU) wParam, IDM_EDIT_CLEAR, iEnable);
         return 0;
      }
      break;

   case WM_COMMAND:
      if ( lParam )
      {
         if ( LOWORD(lParam) == ID_EDIT &&
             (HIWORD(wParam) == EN_ERRSPACE ||
              HIWORD(wParam) == EN_MAXTEXT) )
            MessageBoxW(hwnd, L"Edit control out of space.",
                        szAppName, MB_OK | MB_ICONSTOP);
         return 0;
      }
      else switch ( LOWORD(wParam) )
      {
      case IDM_FILE_NEW:
      case IDM_FILE_OPEN:
      case IDM_FILE_SAVE:
      case IDM_FILE_SAVE_AS:
      case IDM_FILE_PRINT:
         MessageBeep(0);
         return 0;

      case IDM_APP_EXIT:
         SendMessageW(hwnd, WM_CLOSE, 0, 0);
         return 0;

      case IDM_EDIT_UNDO:
         SendMessageW(edit, WM_UNDO, 0, 0);
         return 0;

      case IDM_EDIT_CUT:
         SendMessageW(edit, WM_CUT, 0, 0);
         return 0;

      case IDM_EDIT_COPY:
         SendMessageW(edit, WM_COPY, 0, 0);
         return 0;

      case IDM_EDIT_PASTE:
         SendMessageW(edit, WM_PASTE, 0, 0);
         return 0;

      case IDM_EDIT_CLEAR:
         SendMessageW(edit, WM_CLEAR, 0, 0);
         return 0;

      case IDM_EDIT_SELECT_ALL:
         SendMessageW(edit, EM_SETSEL, 0, -1);
         return 0;

      case IDM_HELP_HELP:
         MessageBoxW(hwnd, L"Help not yet implemented!",
                     szAppName, MB_OK | MB_ICONEXCLAMATION);
         return 0;

      case IDM_APP_ABOUT:
         MessageBoxW(hwnd, L"POPPAD2 (c) Charles Petzold, 1998",
                     szAppName, MB_OK | MB_ICONINFORMATION);
         return 0;
      }
      break;

   case WM_CLOSE:
      if ( IDYES == AskConfirmation(hwnd) )
         DestroyWindow(hwnd);
      return 0;

   case WM_QUERYENDSESSION:
      if ( IDYES == AskConfirmation(hwnd) )
         return 1;
      else
         return 0;

   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(hwnd, message, wParam, lParam);
}