/*----------------------------------------
   BEEPER2.C -- Timer Demo Program No. 2
            (c) Charles Petzold, 1998
  ----------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#define ID_TIMER    1

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
VOID    CALLBACK TimerProc(HWND, UINT, UINT_PTR, DWORD);

int WINAPI wWinMain(_In_     HINSTANCE instance,
                    _In_opt_ HINSTANCE prevInstance,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInstance);
   UNREFERENCED_PARAMETER(cmdLine);

   static PCWSTR  appName = L"Beeper2";
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

   hwnd = CreateWindowW(appName, L"Beeper2 Timer Demo",
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
   switch ( message )
   {
   case WM_CREATE:
      SetTimer(hwnd, ID_TIMER, 1000, TimerProc);
      return 0;

   case WM_DESTROY:
      KillTimer(hwnd, ID_TIMER);
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(hwnd, message, wParam, lParam);
}

VOID CALLBACK TimerProc(HWND hwnd, UINT message, UINT_PTR iTimerID, DWORD dwTime)
{
   UNREFERENCED_PARAMETER(message);
   UNREFERENCED_PARAMETER(iTimerID);
   UNREFERENCED_PARAMETER(dwTime);

   static BOOL flipFlop = FALSE;
   HBRUSH      hBrush;
   HDC         hdc;
   RECT        rc;

   MessageBeep(-1);
   flipFlop = !flipFlop;

   GetClientRect(hwnd, &rc);

   hdc    = GetDC(hwnd);
   hBrush = CreateSolidBrush(flipFlop ? RGB(255, 0, 0) : RGB(0, 0, 255));

   FillRect(hdc, &rc, hBrush);
   ReleaseDC(hwnd, hdc);
   DeleteObject(hBrush);
}