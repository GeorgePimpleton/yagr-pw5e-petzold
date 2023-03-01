/*-----------------------------------------
   BEEPER1.C  -- Timer Demo Program No. 1
             (c) Charles Petzold, 1998
  -----------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#define ID_TIMER    1

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(_In_     HINSTANCE instance,
                    _In_opt_ HINSTANCE prevInstance,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInstance);
   UNREFERENCED_PARAMETER(cmdLine);

   static PCWSTR  appName = L"Beeper1";
   HWND           hwnd;
   MSG            msg;
   WNDCLASSW      wc;

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
      MessageBoxW(NULL, L"Program requires Windows NT!",
                  appName, MB_ICONERROR);
      return 0;
   }

   hwnd = CreateWindowW(appName, L"Beeper1 Timer Demo",
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
   static BOOL flipFlop = FALSE;
   HBRUSH      hBrush;
   HDC         hdc;
   PAINTSTRUCT ps;
   RECT        rc;

   switch ( message )
   {
   case WM_CREATE:
      SetTimer(hwnd, ID_TIMER, 1000, NULL);
      return 0;

   case WM_TIMER:
      MessageBeep(-1);
      flipFlop = !flipFlop;
      InvalidateRect(hwnd, NULL, FALSE);
      return 0;

   case WM_PAINT:
      hdc = BeginPaint(hwnd, &ps);

      GetClientRect(hwnd, &rc);
      hBrush = CreateSolidBrush(flipFlop ? RGB(255, 0, 0) : RGB(0, 0, 255));
      FillRect(hdc, &rc, hBrush);

      EndPaint(hwnd, &ps);
      DeleteObject(hBrush);
      return 0;

   case WM_DESTROY:
      KillTimer(hwnd, ID_TIMER);
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(hwnd, message, wParam, lParam);
}