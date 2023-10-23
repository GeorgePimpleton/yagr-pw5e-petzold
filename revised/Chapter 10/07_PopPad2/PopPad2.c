/*-----------------------------------------------------
   POPPAD2.C -- Popup Editor Version 2 (includes menu)
                (c) Charles Petzold, 1998
  -----------------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "Resource.h"

#define ID_EDIT     1

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

PCWSTR g_appName = L"PopPad2";

int WINAPI wWinMain(_In_     HINSTANCE inst,
                    _In_opt_ HINSTANCE prevInst,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInst);
   UNREFERENCED_PARAMETER(cmdLine);

   HACCEL    accel;
   HWND      wnd;
   MSG       msg;
   WNDCLASSW wc = { 0 };

   wc.style         = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = inst;
   wc.hIcon         = (HICON)   LoadImageW(inst, g_appName, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wc.hbrBackground = (HBRUSH)  (COLOR_WINDOW + 1);
   wc.lpszMenuName  = g_appName;
   wc.lpszClassName = g_appName;

   if ( !RegisterClassW(&wc) )
   {
      MessageBoxW(NULL, L"This program requires Windows NT!", g_appName, MB_ICONERROR);
      return 0;
   }

   wnd = CreateWindowW(g_appName, g_appName,
                       WS_OVERLAPPEDWINDOW,
                       GetSystemMetrics(SM_CXSCREEN) / 4,
                       GetSystemMetrics(SM_CYSCREEN) / 4,
                       GetSystemMetrics(SM_CXSCREEN) / 2,
                       GetSystemMetrics(SM_CYSCREEN) / 2,
                       NULL, NULL, inst, NULL);

   ShowWindow(wnd, showCmd);
   UpdateWindow(wnd);

   accel = LoadAcceleratorsW(inst, g_appName);

   while ( GetMessageW(&msg, NULL, 0, 0) )
   {
      if ( !TranslateAcceleratorW(wnd, accel, &msg) )
      {
         TranslateMessage(&msg);
         DispatchMessageW(&msg);
      }
   }
   return (int) msg.wParam;
}

AskConfirmation(HWND wnd)
{
   return MessageBoxW(wnd, L"Really want to close PopPad2?", g_appName, MB_YESNO | MB_ICONQUESTION);
}

LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static HWND edit;
   int         select;
   int         enable;

   switch ( msg )
   {
   case WM_CREATE:
      edit = CreateWindowW(L"edit", NULL,
                           WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
                           WS_BORDER | ES_LEFT | ES_MULTILINE |
                           ES_AUTOHSCROLL | ES_AUTOVSCROLL,
                           0, 0, 0, 0, wnd, (HMENU) ID_EDIT,
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
         {
            enable = MF_GRAYED;
         }
         else
         {
            enable = MF_ENABLED;
         }

         EnableMenuItem((HMENU) wParam, IDM_EDIT_CUT, enable);
         EnableMenuItem((HMENU) wParam, IDM_EDIT_COPY, enable);
         EnableMenuItem((HMENU) wParam, IDM_EDIT_CLEAR, enable);
         return 0;
      }
      break;

   case WM_COMMAND:
      if ( lParam )
      {
         if ( LOWORD(lParam) == ID_EDIT &&
             (HIWORD(wParam) == EN_ERRSPACE ||
              HIWORD(wParam) == EN_MAXTEXT) )
         {
            MessageBoxW(wnd, L"Edit control out of space.",
                        g_appName, MB_OK | MB_ICONSTOP);
         }
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
         SendMessageW(wnd, WM_CLOSE, 0, 0);
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
         MessageBoxW(wnd, L"Help not yet implemented!",
                     g_appName, MB_OK | MB_ICONEXCLAMATION);
         return 0;

      case IDM_APP_ABOUT:
         MessageBoxW(wnd, L"POPPAD2 (c) Charles Petzold, 1998",
                     g_appName, MB_OK | MB_ICONINFORMATION);
         return 0;
      }
      break;

   case WM_CLOSE:
      if ( IDYES == AskConfirmation(wnd) )
      {
         DestroyWindow(wnd);
      }
      return 0;

   case WM_QUERYENDSESSION:
      if ( IDYES == AskConfirmation(wnd) )
      {
         return 1;
      }
      else
      {
         return 0;
      }

   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(wnd, msg, wParam, lParam);
}