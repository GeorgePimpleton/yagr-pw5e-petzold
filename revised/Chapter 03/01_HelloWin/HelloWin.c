/*------------------------------------------------------------
   HelloWin.c -- Displays "Hello, Windows 98!" in client area
             (c) Charles Petzold, 1998
  ------------------------------------------------------------*/

#define WIN32_MEAN_AND_LEAN

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>

// make sure the Windows multi-media library DLL is loaded for use
#pragma comment(lib, "winmm.lib")

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(_In_     HINSTANCE instance,
                    _In_opt_ HINSTANCE prevInstance,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInstance);
   UNREFERENCED_PARAMETER(cmdLine);

   static PCWSTR clsName  = L"HelloWin";
   static PCWSTR appTitle = L"The Hello Program";
   WNDCLASSEX    wc;

   wc.cbSize        = sizeof(WNDCLASSEXW);
   wc.hInstance     = instance;
   wc.lpszClassName = clsName;
   wc.lpfnWndProc   = WndProc;
   wc.style         = 0;
   wc.hIcon         = (HICON)   LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hIconSm       = (HICON)   LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wc.lpszMenuName  = NULL;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hbrBackground = (HBRUSH)  (COLOR_WINDOW + 1);

   ATOM atom = RegisterClassExW(&wc);

 if ( 0 == atom ) { return 0;  /* premature exit */ }

   HWND hwnd = CreateWindowW((PCWSTR) atom,        // window class name or atom
                             appTitle,             // window caption
                             WS_OVERLAPPEDWINDOW,  // window style
                             CW_USEDEFAULT,        // initial x position
                             CW_USEDEFAULT,        // initial y position
                             CW_USEDEFAULT,        // initial x size
                             CW_USEDEFAULT,        // initial y size
                             NULL,                 // parent window handle
                             NULL,                 // window menu handle
                             instance,            // program instance handle
                             NULL);                // creation parameters

   if ( 0 == hwnd ) { return 0;  /* premature exit*/ }

   ShowWindow(hwnd, showCmd);

   if ( 0 == UpdateWindow(hwnd) ) { return 0;  /* premature exit*/ }

   MSG  msg;
   BOOL ret;  // three states: -1, 0 or non-zero

   while ( 0 != (ret = GetMessage(&msg, NULL, 0, 0)) )
   {
      if ( -1 == ret )
      {
         // handle the error and/or exit
         // for error call GetLastError();
         return 0;
      }
      else
      {
         TranslateMessage(&msg);
         DispatchMessageW(&msg);
      }
   }

   return (int) msg.wParam;  // WM_QUIT
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   switch ( message )
   {
   case WM_CREATE:
      PlaySoundW(L"HelloWin.wav", NULL, SND_FILENAME | SND_ASYNC);
      return 0;  // message processed

   case WM_PAINT:
   {
      PAINTSTRUCT ps;
      HDC         hdc = BeginPaint(hwnd, &ps);
      RECT        rect;

      GetClientRect(hwnd, &rect);

      DrawTextW(hdc, L"Hello, Windows 98!", -1, &rect,
                DT_SINGLELINE | DT_CENTER | DT_VCENTER);

      EndPaint(hwnd, &ps);
   }
   return 0;  // message processed

   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;  // message processed

   default:
      return DefWindowProcW(hwnd, message, wParam, lParam);
   }
}