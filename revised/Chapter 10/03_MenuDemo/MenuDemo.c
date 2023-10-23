/*-----------------------------------------
   MENUDEMO.C -- Menu Demonstration
                 (c) Charles Petzold, 1998
  -----------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>

#include "Resource.h"

#define ID_TIMER 1

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

PCWSTR appName = L"MenuDemo";

int WINAPI wWinMain(_In_     HINSTANCE inst,
                    _In_opt_ HINSTANCE prevInst,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInst);
   UNREFERENCED_PARAMETER(cmdLine);

   HWND      wnd;
   MSG       msg;
   WNDCLASSW wc = { 0 };

   wc.style         = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = inst;
   wc.hIcon         = (HICON)   LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wc.hbrBackground = (HBRUSH)  (COLOR_WINDOW + 1);
   wc.lpszMenuName  = appName;
   wc.lpszClassName = appName;

   if ( !RegisterClassW(&wc) )
   {
      MessageBoxW(NULL, L"This program requires Windows NT!", appName, MB_ICONERROR);
      return 0;
   }

   wnd = CreateWindowW(appName, L"Menu Demonstration",
                       WS_OVERLAPPEDWINDOW,
                       CW_USEDEFAULT, CW_USEDEFAULT,
                       CW_USEDEFAULT, CW_USEDEFAULT,
                       NULL, NULL, inst, NULL);

   ShowWindow(wnd, showCmd);
   UpdateWindow(wnd);

   while ( GetMessageW(&msg, NULL, 0, 0) )
   {
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
   }
   return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static int color[ 5 ] = { WHITE_BRUSH, LTGRAY_BRUSH, GRAY_BRUSH, DKGRAY_BRUSH, BLACK_BRUSH };
   static int selection  = IDM_BKGND_WHITE;
   HMENU      menu;

   switch ( msg )
   {
   case WM_COMMAND:
      menu    = GetMenu(wnd);
      int  id = GET_WM_COMMAND_ID(wParam, LPARAM);

      switch ( id )
      {
      case IDM_FILE_NEW:
      case IDM_FILE_OPEN:
      case IDM_FILE_SAVE:
      case IDM_FILE_SAVE_AS:
         MessageBeep(0);
         return 0;

      case IDM_APP_EXIT:
         SendMessageW(wnd, WM_CLOSE, 0, 0);
         return 0;

      case IDM_EDIT_UNDO:
      case IDM_EDIT_CUT:
      case IDM_EDIT_COPY:
      case IDM_EDIT_PASTE:
      case IDM_EDIT_CLEAR:
         MessageBeep(0);
         return 0;

      case IDM_BKGND_WHITE:         // note: logic below
      case IDM_BKGND_LTGRAY:        //   assumes that IDM_WHITE
      case IDM_BKGND_GRAY:          //   through IDM_BLACK are
      case IDM_BKGND_DKGRAY:        //   consecutive numbers in
      case IDM_BKGND_BLACK:         //   the order shown here.
         CheckMenuItem(menu, selection, MF_UNCHECKED);
         selection = id;
         CheckMenuItem(menu, selection, MF_CHECKED);

         SetClassLongPtrW(wnd, GCLP_HBRBACKGROUND,
                          (LONG_PTR) GetStockObject(color[ id - IDM_BKGND_WHITE ]));

         InvalidateRect(wnd, NULL, TRUE);
         return 0;

      case IDM_TIMER_START:
         if ( SetTimer(wnd, ID_TIMER, 1000, NULL) )
         {
            EnableMenuItem(menu, IDM_TIMER_START, MF_GRAYED);
            EnableMenuItem(menu, IDM_TIMER_STOP, MF_ENABLED);
         }
         return 0;

      case IDM_TIMER_STOP:
         KillTimer(wnd, ID_TIMER);
         EnableMenuItem(menu, IDM_TIMER_START, MF_ENABLED);
         EnableMenuItem(menu, IDM_TIMER_STOP, MF_GRAYED);
         return 0;

      case IDM_APP_HELP:
         MessageBoxW(wnd, L"Help not yet implemented!", appName, MB_ICONEXCLAMATION | MB_OK);
         return 0;

      case IDM_APP_ABOUT:
         MessageBoxW(wnd, L"Menu Demonstration Program\n"
                     L"(c) Charles Petzold, 1998",
                     appName, MB_ICONINFORMATION | MB_OK);
         return 0;
      }
      break;

   case WM_TIMER:
      MessageBeep(0);
      return 0;

   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(wnd, msg, wParam, lParam);
}