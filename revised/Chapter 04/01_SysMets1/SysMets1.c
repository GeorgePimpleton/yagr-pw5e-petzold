/*----------------------------------------------------
   SYSMETS1.C -- System Metrics Display Program No. 1
                 (c) Charles Petzold, 1998
  ----------------------------------------------------*/

  #define WIN32_MEAN_AND_LEAN

#include <windows.h>
#include <windowsx.h>

#include "SysMets.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(_In_     HINSTANCE inst,
                    _In_opt_ HINSTANCE prevInst,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInst);
   UNREFERENCED_PARAMETER(cmdLine);

   static PCWSTR appName  = L"SysMets1";
   static PCWSTR appTitle = L"Get System Metrics No. 1";
   HWND          wnd;
   MSG           msg;
   WNDCLASSW     wc = { 0 };

   wc.style         = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = inst;
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

   wnd = CreateWindowW(appName, appTitle,
                       WS_OVERLAPPEDWINDOW,
                       CW_USEDEFAULT, CW_USEDEFAULT,
                       CW_USEDEFAULT, CW_USEDEFAULT,
                       NULL, NULL, inst, NULL);

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
   static int  xChar;
   static int  xCaps;
   static int  yChar;
   HDC         dc;
   PAINTSTRUCT ps;
   WCHAR       buffer[ 10 ] = { L"" };
   TEXTMETRICW tm;

   switch ( msg )
   {
   case WM_CREATE:
      dc = GetDC(wnd);

      GetTextMetricsW(dc, &tm);

      xChar = tm.tmAveCharWidth;
      xCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * xChar / 2;
      yChar = tm.tmHeight + tm.tmExternalLeading;

      ReleaseDC(wnd, dc);
      return 0;

   case WM_PAINT:
      dc = BeginPaint(wnd, &ps);

      for ( unsigned i = 0; i < NUMLINES; i++ )
      {
         TextOutW(dc, 1, yChar * i,
                  sysmetrics[ i ].label,
                  lstrlenW(sysmetrics[ i ].label));

         TextOutW(dc, 22 * xCaps + 1, yChar * i,
                  sysmetrics[ i ].desc,
                  lstrlenW(sysmetrics[ i ].desc));

         SetTextAlign(dc, TA_RIGHT | TA_TOP);

         TextOutW(dc, 22 * xCaps + 40 * xChar + 1, yChar * i, buffer,
                  wsprintfW(buffer, L"%5d",
                            GetSystemMetrics(sysmetrics[ i ].index)));

         SetTextAlign(dc, TA_LEFT | TA_TOP);
      }
      EndPaint(wnd, &ps);
      return 0;

   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(wnd, msg, wParam, lParam);
}