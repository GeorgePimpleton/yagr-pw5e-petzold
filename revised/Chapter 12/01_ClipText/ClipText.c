/*-----------------------------------------
   CLIPTEXT.C -- The Clipboard and Text
                 (c) Charles Petzold, 1998
  -----------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <malloc.h>
#include "Resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

WCHAR defaultText[ ] = L"Default Text";
WCHAR caption[ ]     = L"Clipboard Text Transfers";

int WINAPI wWinMain(_In_     HINSTANCE inst,
                    _In_opt_ HINSTANCE prevInst,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(cmdLine);

   static WCHAR appName[ ] = L"ClipText";
   HACCEL       accel;
   HWND         wnd;
   MSG          msg;
   WNDCLASSW    wc;

   wc.style         = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = inst;
   wc.hIcon         = (HICON)   LoadImageW(inst, appName, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wc.hbrBackground = (HBRUSH)  (COLOR_WINDOW + 1);
   wc.lpszMenuName  = appName;
   wc.lpszClassName = appName;

   if ( !RegisterClassW(&wc) )
   {
      MessageBoxW(NULL, L"This program requires Windows NT!", appName, MB_ICONERROR);
      return 0;
   }

   wnd = CreateWindowW(appName, caption,
                       WS_OVERLAPPEDWINDOW,
                       CW_USEDEFAULT, CW_USEDEFAULT,
                       CW_USEDEFAULT, CW_USEDEFAULT,
                       NULL, NULL, inst, NULL);

   ShowWindow(wnd, showCmd);
   UpdateWindow(wnd);

   accel = LoadAcceleratorsW(inst, appName);

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

LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static PWSTR text;
   BOOL         enable;
   HGLOBAL      global;
   HDC          dc;
   PWSTR        strGlobal;
   PAINTSTRUCT  ps;
   RECT         rect;

   switch ( msg )
   {
   case WM_CREATE:
      SendMessageW(wnd, WM_COMMAND, IDM_EDIT_RESET, 0);
      return 0;

   case WM_INITMENUPOPUP:
      EnableMenuItem((HMENU) wParam, IDM_EDIT_PASTE,
                     IsClipboardFormatAvailable(CF_UNICODETEXT) ? MF_ENABLED : MF_GRAYED);

      enable = text ? MF_ENABLED : MF_GRAYED;

      EnableMenuItem((HMENU) wParam, IDM_EDIT_CUT, enable);
      EnableMenuItem((HMENU) wParam, IDM_EDIT_COPY, enable);
      EnableMenuItem((HMENU) wParam, IDM_EDIT_CLEAR, enable);
      break;

   case WM_COMMAND:
      switch ( LOWORD(wParam) )
      {
      case IDM_EDIT_PASTE:
         OpenClipboard(wnd);

         if ( global = GetClipboardData(CF_UNICODETEXT) )
         {
            strGlobal = (PWSTR) GlobalLock(global);

            if ( text )
            {
               free(text);
               text = NULL;
            }
            text = (PWSTR) malloc(GlobalSize(global));
            lstrcpyW(text, strGlobal);
            InvalidateRect(wnd, NULL, TRUE);
         }
         CloseClipboard( );
         return 0;

      case IDM_EDIT_CUT:
      case IDM_EDIT_COPY:
         if ( !text )
         {
            return 0;
         }

         global    = GlobalAlloc(GHND | GMEM_SHARE, (lstrlenW(text) + 1) * sizeof(WCHAR));
         strGlobal = (PWSTR) GlobalLock(global);
         lstrcpyW(strGlobal, text);
         GlobalUnlock(global);

         OpenClipboard(wnd);
         EmptyClipboard( );
         SetClipboardData(CF_UNICODETEXT, global);
         CloseClipboard( );

         if ( LOWORD(wParam) == IDM_EDIT_COPY )
         {
            return 0;
         }
         // fall through for IDM_EDIT_CUT

      case IDM_EDIT_CLEAR:
         if ( text )
         {
            free(text);
            text = NULL;
         }
         InvalidateRect(wnd, NULL, TRUE);
         return 0;

      case IDM_EDIT_RESET:
         if ( text )
         {
            free(text);
            text = NULL;
         }

         text = (PWSTR) malloc((lstrlen(defaultText) + 1) * sizeof(TCHAR));
         lstrcpy(text, defaultText);
         InvalidateRect(wnd, NULL, TRUE);
         return 0;
      }
      break;

   case WM_PAINT:
      dc = BeginPaint(wnd, &ps);

      GetClientRect(wnd, &rect);

      if ( text != NULL )
      {
         DrawTextW(dc, text, -1, &rect, DT_EXPANDTABS | DT_WORDBREAK);
      }

      EndPaint(wnd, &ps);
      return 0;

   case WM_DESTROY:
      if ( text )
      {
         free(text);
      }

      PostQuitMessage(0);
      return 0;
   }
   return DefWindowProcW(wnd, msg, wParam, lParam);
}
