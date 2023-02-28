/*----------------------------------------------------
   SYSMETS1.C -- System Metrics Display Program No. 1
             (c) Charles Petzold, 1998
  ----------------------------------------------------*/

  #define WIN32_MEAN_AND_LEAN

#include <windows.h>
#include <windowsx.h>

#include "SysMets.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(_In_     HINSTANCE instance,
                    _In_opt_ HINSTANCE prevInstance,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInstance);
   UNREFERENCED_PARAMETER(cmdLine);

   static PCWSTR appName  = L"SysMets1";
   static PCWSTR appTitle = L"Get System Metrics No. 1";
   HWND          hwnd;
   MSG           msg;
   WNDCLASSW     wc;

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
      MessageBoxW(NULL, L"This program requires Windows NT!", appName, MB_ICONERROR);
      return 0;
   }

   hwnd = CreateWindowW(appName, appTitle,
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
   static int  cxChar;
   static int  cxCaps;
   static int  cyChar;
   HDC         hdc;
   PAINTSTRUCT ps;
   WCHAR       buffer [ 10 ];
   TEXTMETRICW tm;

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
         TextOutW(hdc, 1, cyChar * i,
                  sysmetrics[ i ].label,
                  lstrlenW(sysmetrics[ i ].label));

         TextOutW(hdc, 22 * cxCaps + 1, cyChar * i,
                  sysmetrics[ i ].desc,
                  lstrlenW(sysmetrics[ i ].desc));

         SetTextAlign(hdc, TA_RIGHT | TA_TOP);

         TextOutW(hdc, 22 * cxCaps + 40 * cxChar + 1, cyChar * i, buffer,
                  wsprintfW(buffer, L"%5d",
                            GetSystemMetrics(sysmetrics[ i ].index)));

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