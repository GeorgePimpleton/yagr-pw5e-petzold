/*----------------------------------------------------
   SYSMETS1.C -- System Metrics Display Program No. 1
             (c) Charles Petzold, 1998
  ----------------------------------------------------*/

#include <windows.h>
#include <windowsx.h>
#include "SysMets.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(_In_     HINSTANCE hInstance,
                    _In_opt_ HINSTANCE hPrevInstance,
                    _In_     PWSTR     pCmdLine,
                    _In_     int       nShowCmd)
{
   UNREFERENCED_PARAMETER(hPrevInstance);
   UNREFERENCED_PARAMETER(pCmdLine);

   static WCHAR szAppName[] = L"SysMets1";
   HWND         hwnd;
   MSG          msg;
   WNDCLASSW    wndclass;

   wndclass.style         = CS_HREDRAW | CS_VREDRAW;
   wndclass.lpfnWndProc   = WndProc;
   wndclass.cbClsExtra    = 0;
   wndclass.cbWndExtra    = 0;
   wndclass.hInstance     = hInstance;
   wndclass.hIcon         = (HICON)   LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wndclass.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wndclass.hbrBackground = (HBRUSH)  (COLOR_WINDOW + 1);
   wndclass.lpszMenuName  = NULL;
   wndclass.lpszClassName = szAppName;

   if ( !RegisterClassW(&wndclass) )
   {
      MessageBoxW(NULL, L"This program requires Windows NT!", szAppName, MB_ICONERROR);
      return 0;
   }

   hwnd = CreateWindowW(szAppName, L"Get System Metrics No. 1",
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
   return (int) msg.wParam;  // WM_QUIT
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   static int  cxChar;
   static int  cxCaps;
   static int  cyChar;
   HDC         hdc;
   PAINTSTRUCT ps;
   WCHAR       szBuffer[ 10 ];
   TEXTMETRIC  tm;

   switch ( message )
   {
   case WM_CREATE:
      hdc = GetDC(hwnd);

      GetTextMetricsW(hdc, &tm);
      cxChar = tm.tmAveCharWidth;
      cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2;
      cyChar = tm.tmHeight + tm.tmExternalLeading;

      ReleaseDC(hwnd, hdc);
      return 0;

   case WM_PAINT:
      hdc = BeginPaint(hwnd, &ps);

      for ( unsigned i = 0; i < NUMLINES; i++ )
      {
         TextOutW(hdc, 0, cyChar * i,
                  sysmetrics[ i ].szLabel,
                  lstrlenW(sysmetrics[ i ].szLabel));

         TextOutW(hdc, 22 * cxCaps, cyChar * i,
                  sysmetrics[ i ].szDesc,
                  lstrlenW(sysmetrics[ i ].szDesc));

         SetTextAlign(hdc, TA_RIGHT | TA_TOP);

         TextOutW(hdc, 22 * cxCaps + 40 * cxChar, cyChar * i, szBuffer,
                  wsprintfW(szBuffer, L"%5d",
                            GetSystemMetrics(sysmetrics[ i ].iIndex)));

         SetTextAlign(hdc, TA_LEFT | TA_TOP);
      }
      EndPaint(hwnd, &ps);
      return 0;

   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(hwnd, message, wParam, lParam);
}