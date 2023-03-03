/*----------------------------------------
   POPMENU.C -- Popup Menu Demonstration
            (c) Charles Petzold, 1998
  ----------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include "Resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE inst;
PCWSTR    appName = L"PopMenu";

int WINAPI wWinMain(_In_     HINSTANCE instance,
                    _In_opt_ HINSTANCE prevInstance,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInstance);
   UNREFERENCED_PARAMETER(cmdLine);

   HWND        hwnd;
   MSG         msg;
   WNDCLASSW   wc;

   wc.style         = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = instance;
   wc.hIcon         = (HICON)   LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wc.hbrBackground = (HBRUSH)  (COLOR_WINDOW + 1);
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = appName;

   if ( !RegisterClassW(&wc) )
   {
      MessageBoxW(NULL, L"This program requires Windows NT!",
                  appName, MB_ICONERROR);
      return 0;
   }

   inst = instance;

   hwnd = CreateWindowW(appName, L"Popup Menu Demonstration",
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        NULL, NULL, instance, NULL);

   ShowWindow(hwnd, showCmd);
   UpdateWindow(hwnd);

   while ( GetMessageW(&msg, NULL, 0, 0) )
   {
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
   }
   return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   static HMENU menu;
   static int   color[ 5 ] = { WHITE_BRUSH,  LTGRAY_BRUSH, GRAY_BRUSH, DKGRAY_BRUSH, BLACK_BRUSH };
   static int   selection  = IDM_BKGND_WHITE;
   POINT        point;
   int          id;

   switch ( message )
   {
   case WM_CREATE:
      menu = LoadMenuW(inst, appName);
      menu = GetSubMenu(menu, 0);
      return 0;

   case WM_RBUTTONUP:
      point.x = LOWORD(lParam);
      point.y = HIWORD(lParam);
      ClientToScreen(hwnd, &point);

      TrackPopupMenu(menu, TPM_RIGHTBUTTON, point.x, point.y,
                     0, hwnd, NULL);
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

      case IDM_BKGND_WHITE:         // Note: Logic below
      case IDM_BKGND_LTGRAY:        //   assumes that IDM_WHITE
      case IDM_BKGND_GRAY:          //   through IDM_BLACK are
      case IDM_BKGND_DKGRAY:        //   consecutive numbers in
      case IDM_BKGND_BLACK:         //   the order shown here.

         CheckMenuItem(menu, selection, MF_UNCHECKED);
         selection = id;
         CheckMenuItem(menu, selection, MF_CHECKED);

         SetClassLongPtrW(hwnd,
                          GCLP_HBRBACKGROUND,
                          (LONG_PTR) GetStockObject(color[ id - IDM_BKGND_WHITE ]));

         InvalidateRect(hwnd, NULL, TRUE);
         return 0;

      case IDM_APP_ABOUT:
         MessageBoxW(hwnd, L"Popup Menu Demonstration Program\n"
                     L"(c) Charles Petzold, 1998",
                     appName, MB_ICONINFORMATION | MB_OK);
         return 0;

      case IDM_APP_EXIT:
         SendMessageW(hwnd, WM_CLOSE, 0, 0);
         return 0;

      case IDM_APP_HELP:
         MessageBoxW(hwnd, L"Help not yet implemented!",
                     appName, MB_ICONEXCLAMATION | MB_OK);
         return 0;
      }
      break;

   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(hwnd, message, wParam, lParam);
}