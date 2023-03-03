/*-----------------------------------------
   POORMENU.C -- The Poor Person's Menu
             (c) Charles Petzold, 1998
  -----------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#define IDM_SYS_ABOUT   1
#define IDM_SYS_HELP    2
#define IDM_SYS_REMOVE  3

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

PCWSTR appName = L"PoorMenu";

int WINAPI wWinMain(_In_     HINSTANCE instance,
                    _In_opt_ HINSTANCE prevInstance,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInstance);
   UNREFERENCED_PARAMETER(cmdLine);

   HMENU     menu;
   HWND      hwnd;
   MSG       msg;
   WNDCLASSW wc;

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

   hwnd = CreateWindowW(appName, L"The Poor-Person's Menu",
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        NULL, NULL, instance, NULL);

   menu = GetSystemMenu(hwnd, FALSE);

   AppendMenuW(menu, MF_SEPARATOR, 0, NULL);
   AppendMenuW(menu, MF_STRING, IDM_SYS_ABOUT, L"About...");
   AppendMenuW(menu, MF_STRING, IDM_SYS_HELP, L"Help...");
   AppendMenuW(menu, MF_STRING, IDM_SYS_REMOVE, L"Remove Additions");

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
   switch ( message )
   {
   case WM_SYSCOMMAND:
      switch ( LOWORD(wParam) )
      {
      case IDM_SYS_ABOUT:
         MessageBoxW(hwnd, L"A Poor-Person's Menu Program\n"
                     L"(c) Charles Petzold, 1998",
                     appName, MB_OK | MB_ICONINFORMATION);
         return 0;

      case IDM_SYS_HELP:
         MessageBoxW(hwnd, L"Help not yet implemented!",
                     appName, MB_OK | MB_ICONEXCLAMATION);
         return 0;

      case IDM_SYS_REMOVE:
         GetSystemMenu(hwnd, TRUE);
         return 0;
      }
      break;

   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(hwnd, message, wParam, lParam);
}