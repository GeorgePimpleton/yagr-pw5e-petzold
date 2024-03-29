/*----------------------------------------
   POPMENU.C -- Popup Menu Demonstration
                (c) Charles Petzold, 1998
  ----------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>

#include "Resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE g_inst;
PCWSTR    g_appName = L"PopMenu";

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
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = g_appName;

   if ( !RegisterClassW(&wc) )
   {
      MessageBoxW(NULL, L"This program requires Windows NT!", g_appName, MB_ICONERROR);
      return 0;
   }

   g_inst = inst;

   wnd = CreateWindowW(g_appName, L"Popup Menu Demonstration",
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
   static HMENU menu;
   static int   color[ 5 ] = { WHITE_BRUSH, LTGRAY_BRUSH, GRAY_BRUSH, DKGRAY_BRUSH, BLACK_BRUSH };
   static int   selection  = IDM_BKGND_WHITE;
   POINT        point      = { 0 };
   int          id;

   switch ( msg )
   {
   case WM_CREATE:
      menu = LoadMenuW(g_inst, g_appName);
      menu = GetSubMenu(menu, 0);
      return 0;

   case WM_RBUTTONUP:
      point.x = LOWORD(lParam);
      point.y = HIWORD(lParam);
      ClientToScreen(wnd, &point);

      TrackPopupMenu(menu, TPM_RIGHTBUTTON, point.x, point.y, 0, wnd, NULL);
      return 0;

   case WM_COMMAND:
      id = GET_WM_COMMAND_ID(wParam, lParam);
      switch ( id )
      {
      case IDM_FILE_NEW:
      case IDM_FILE_OPEN:
      case IDM_FILE_SAVE:
      case IDM_FILE_SAVE_AS:
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

      case IDM_APP_ABOUT:
         MessageBoxW(wnd, L"Popup Menu Demonstration Program\n"
                          L"(c) Charles Petzold, 1998",
                          g_appName, MB_ICONINFORMATION | MB_OK);
         return 0;

      case IDM_APP_EXIT:
         SendMessageW(wnd, WM_CLOSE, 0, 0);
         return 0;

      case IDM_APP_HELP:
         MessageBoxW(wnd, L"Help not yet implemented!",
                     g_appName, MB_ICONEXCLAMATION | MB_OK);
         return 0;
      }
      break;

   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(wnd, msg, wParam, lParam);
}