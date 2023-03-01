/*------------------------------------------
   WHATCLR.C -- Displays Color Under Cursor
            (c) Charles Petzold, 1998
  ------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#define ID_TIMER    1

void             FindWindowSize(int*, int*);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(_In_     HINSTANCE instance,
                    _In_opt_ HINSTANCE prevInstance,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInstance);
   UNREFERENCED_PARAMETER(cmdLine);

   static PCWSTR  appName = L"WhatClr";
   HWND           hwnd;
   int            cxWindow;
   int            cyWindow;
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
      MessageBoxW(NULL, L"This program requires Windows NT!",
                  appName, MB_ICONERROR);
      return 0;
   }

   FindWindowSize(&cxWindow, &cyWindow);

   hwnd = CreateWindowW(appName, L"What Color",
                        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_BORDER,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        cxWindow, cyWindow,
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

void FindWindowSize(int* xWindow, int* yWindow)
{
   HDC         hdcScreen;
   TEXTMETRICW tm;

   hdcScreen = CreateICW(L"DISPLAY", NULL, NULL, NULL);
   GetTextMetricsW(hdcScreen, &tm);
   DeleteDC(hdcScreen);

   *xWindow = 2 * GetSystemMetrics(SM_CXBORDER) +
             12 * tm.tmAveCharWidth;

   *yWindow = 2 * GetSystemMetrics(SM_CYBORDER) +
                  GetSystemMetrics(SM_CYCAPTION) +
              2 * tm.tmHeight;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   static COLORREF cr;
   static COLORREF crLast;
   static HDC      screen;
   HDC             hdc;
   PAINTSTRUCT     ps;
   POINT           pt;
   RECT            rc;
   WCHAR           buffer[ 16 ];

   switch ( message )
   {
   case WM_CREATE:
      screen = CreateDCW(L"DISPLAY", NULL, NULL, NULL);
      SetTimer(hwnd, ID_TIMER, 100, NULL);
      return 0;

   case WM_DISPLAYCHANGE:
      DeleteDC(screen);
      screen = CreateDCW(L"DISPLAY", NULL, NULL, NULL);
      return 0;

   case WM_TIMER:
      GetCursorPos(&pt);
      cr = GetPixel(screen, pt.x, pt.y);

      if ( cr != crLast )
      {
         crLast = cr;
         InvalidateRect(hwnd, NULL, FALSE);
      }
      return 0;

   case WM_PAINT:
      hdc = BeginPaint(hwnd, &ps);

      GetClientRect(hwnd, &rc);

      wsprintfW(buffer, L"  %02X %02X %02X  ",
                GetRValue(cr), GetGValue(cr), GetBValue(cr));

      DrawText(hdc, buffer, -1, &rc,
               DT_SINGLELINE | DT_CENTER | DT_VCENTER);

      EndPaint(hwnd, &ps);
      return 0;

   case WM_DESTROY:
      DeleteDC(screen);
      KillTimer(hwnd, ID_TIMER);
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(hwnd, message, wParam, lParam);
}