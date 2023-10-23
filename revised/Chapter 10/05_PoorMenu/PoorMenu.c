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

PCWSTR g_appName = L"PoorMenu";

int WINAPI wWinMain(_In_     HINSTANCE inst,
                    _In_opt_ HINSTANCE prevInst,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInst);
   UNREFERENCED_PARAMETER(cmdLine);

   HMENU     menu;
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

   wnd = CreateWindowW(g_appName, L"The Poor-Person's Menu",
                       WS_OVERLAPPEDWINDOW,
                       CW_USEDEFAULT, CW_USEDEFAULT,
                       CW_USEDEFAULT, CW_USEDEFAULT,
                       NULL, NULL, inst, NULL);

   menu = GetSystemMenu(wnd, FALSE);

   AppendMenuW(menu, MF_SEPARATOR, 0, NULL);
   AppendMenuW(menu, MF_STRING, IDM_SYS_ABOUT, L"About...");
   AppendMenuW(menu, MF_STRING, IDM_SYS_HELP, L"Help...");
   AppendMenuW(menu, MF_STRING, IDM_SYS_REMOVE, L"Remove Additions");

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
   switch ( msg )
   {
   case WM_SYSCOMMAND:
      switch ( LOWORD(wParam) )
      {
      case IDM_SYS_ABOUT:
         MessageBoxW(wnd, L"A Poor-Person's Menu Program\n"
                     L"(c) Charles Petzold, 1998",
                     g_appName, MB_OK | MB_ICONINFORMATION);
         return 0;

      case IDM_SYS_HELP:
         MessageBoxW(wnd, L"Help not yet implemented!",
                     g_appName, MB_OK | MB_ICONEXCLAMATION);
         return 0;

      case IDM_SYS_REMOVE:
         GetSystemMenu(wnd, TRUE);
         return 0;
      }
      break;

   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(wnd, msg, wParam, lParam);
}