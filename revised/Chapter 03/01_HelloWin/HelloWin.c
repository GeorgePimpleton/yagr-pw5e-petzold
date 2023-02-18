/*------------------------------------------------------------
   HelloWin.c -- Displays "Hello, Windows 98!" in client area
             (c) Charles Petzold, 1998
  ------------------------------------------------------------*/

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>

// make sure the Windows multi-media library DLL is loaded for use
#pragma comment(lib, "winmm.lib")

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(_In_     HINSTANCE hInstance,
                    _In_opt_ HINSTANCE hPrevInstance,
                    _In_     PWSTR     pCmdLine,
                    _In_     int       nShowCmd)
{
   UNREFERENCED_PARAMETER(hPrevInstance);
   UNREFERENCED_PARAMETER(pCmdLine);

   static WCHAR app_name[] = L"HelloWin";
   WNDCLASSEX   wndclassex;

   wndclassex.cbSize        = sizeof(WNDCLASSEXW);
   wndclassex.hInstance     = hInstance;
   wndclassex.lpszClassName = app_name;
   wndclassex.lpfnWndProc   = WndProc;
   wndclassex.style         = 0;
   wndclassex.hIcon         = (HICON)   LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wndclassex.hIconSm       = (HICON)   LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wndclassex.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wndclassex.lpszMenuName  = NULL;
   wndclassex.cbClsExtra    = 0;
   wndclassex.cbWndExtra    = 0;
   wndclassex.hbrBackground = (HBRUSH)  (COLOR_WINDOW + 1);

   ATOM atom = RegisterClassExW(&wndclassex);

 if ( 0 == atom ) { return 0;  /* premature exit */ }

   HWND hwnd = CreateWindowW((PCWSTR) atom,        // window class name or atom
                             L"The Hello Program", // window caption
                             WS_OVERLAPPEDWINDOW,  // window style
                             CW_USEDEFAULT,        // initial x position
                             CW_USEDEFAULT,        // initial y position
                             CW_USEDEFAULT,        // initial x size
                             CW_USEDEFAULT,        // initial y size
                             NULL,                 // parent window handle
                             NULL,                 // window menu handle
                             hInstance,            // program instance handle
                             NULL);                // creation parameters

   if ( 0 == hwnd ) { return 0;  /* premature exit*/ }

   ShowWindow(hwnd, nShowCmd);

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
         (void) DispatchMessageW(&msg);
      }
   }
   return (int) msg.wParam;  // WM_QUIT
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   switch ( message )
   {
   case WM_CREATE:
      (void) PlaySoundW(L"HelloWin.wav", NULL, SND_FILENAME | SND_ASYNC);
      return 0;  // message processed

   case WM_PAINT:
   {
      PAINTSTRUCT ps;
      HDC         hdc = BeginPaint(hwnd, &ps);
      RECT        rect;
      (void) GetClientRect(hwnd, &rect);

      (void) DrawTextW(hdc, L"Hello, Windows 98!", -1, &rect,
                       DT_SINGLELINE | DT_CENTER | DT_VCENTER);

      (void) EndPaint(hwnd, &ps);
   }
   return 0;  // message processed

   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;  // message processed

   default:
      return DefWindowProcW(hwnd, message, wParam, lParam);
   }
}
