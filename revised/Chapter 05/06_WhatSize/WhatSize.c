/*-----------------------------------------
   WHATSIZE.C -- What Size is the Window?
             (c) Charles Petzold, 1998
  -----------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(_In_     HINSTANCE hInstance,
                    _In_opt_ HINSTANCE hPrevInstance,
                    _In_     PWSTR     pCmdLine,
                    _In_     int       nShowCmd)
{
   UNREFERENCED_PARAMETER(hPrevInstance);
   UNREFERENCED_PARAMETER(pCmdLine);

   static PCWSTR  szAppName = L"WhatSize";
   HWND           hwnd;
   MSG            msg;
   WNDCLASSW      wc;

   wc.style         = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = hInstance;
   wc.hIcon         = (HICON)   LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wc.hbrBackground = (HBRUSH)  (COLOR_WINDOW + 1);
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = szAppName;

   if ( !RegisterClassW(&wc) )
   {
      MessageBoxW(NULL, L"This program requires Windows NT!",
                  szAppName, MB_ICONERROR);
      return 0;
   }

   hwnd = CreateWindowW(szAppName, L"What Size is the Window?",
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        NULL, NULL, hInstance, NULL);

   ShowWindow(hwnd, nShowCmd);
   UpdateWindow(hwnd);

   while ( GetMessageW(&msg, NULL, 0, 0) )
   {
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
   }
   return (int) msg.wParam;
}

void Show(HWND hwnd, HDC hdc, int xText, int yText, int iMapMode, PCWSTR szMapMode)
{
   WCHAR szBuffer[ 60 ];
   RECT  rect;

   SaveDC(hdc);

   SetMapMode(hdc, iMapMode);
   GetClientRect(hwnd, &rect);
   DPtoLP(hdc, (PPOINT) &rect, 2);

   RestoreDC(hdc, -1);

   TextOutW(hdc, xText, yText, szBuffer,
            wsprintfW(szBuffer, L"%-20s %7d %7d %7d %7d", szMapMode,
            rect.left, rect.right, rect.top, rect.bottom));
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   static PCWSTR  heading   = L"Mapping Mode            Left   Right     Top  Bottom";
   static PCWSTR  underline = L"------------            ----   -----     ---  ------";
   static int     cxChar;
   static int     cyChar;
   HDC            hdc;
   PAINTSTRUCT    ps;
   TEXTMETRICW    tm;

   switch ( message )
   {
   case WM_CREATE:
      hdc = GetDC(hwnd);
      SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));

      GetTextMetrics(hdc, &tm);
      cxChar = tm.tmAveCharWidth;
      cyChar = tm.tmHeight + tm.tmExternalLeading;

      ReleaseDC(hwnd, hdc);
      return 0;

   case WM_PAINT:
      hdc = BeginPaint(hwnd, &ps);
      SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));

      SetMapMode(hdc, MM_ANISOTROPIC);
      SetWindowExtEx(hdc, 1, 1, NULL);
      SetViewportExtEx(hdc, cxChar, cyChar, NULL);

      TextOut(hdc, 1, 1, heading, lstrlen(heading));
      TextOut(hdc, 1, 2, underline, lstrlen(underline));

      Show(hwnd, hdc, 1, 3, MM_TEXT,      L"TEXT (pixels)");
      Show(hwnd, hdc, 1, 4, MM_LOMETRIC,  L"LOMETRIC (.1 mm)");
      Show(hwnd, hdc, 1, 5, MM_HIMETRIC,  L"HIMETRIC (.01 mm)");
      Show(hwnd, hdc, 1, 6, MM_LOENGLISH, L"LOENGLISH (.01 in)");
      Show(hwnd, hdc, 1, 7, MM_HIENGLISH, L"HIENGLISH (.001 in)");
      Show(hwnd, hdc, 1, 8, MM_TWIPS,     L"TWIPS (1/1440 in)");

      EndPaint(hwnd, &ps);
      return 0;

   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(hwnd, message, wParam, lParam);
}