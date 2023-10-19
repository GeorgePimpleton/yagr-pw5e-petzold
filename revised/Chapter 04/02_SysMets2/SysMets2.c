/*----------------------------------------------------
   SYSMETS2.C -- System Metrics Display Program No. 2
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

   static PCWSTR appName  = L"SysMets2";
   static PCWSTR appTitle = L"Get System Metrics No. 2";
   WNDCLASSW     wc       = { 0 };

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

   HWND hwnd = CreateWindowW(appName, appTitle,
                             WS_OVERLAPPEDWINDOW | WS_VSCROLL,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             NULL, NULL, inst, NULL);

   ShowWindow(hwnd, showCmd);
   UpdateWindow(hwnd);

   MSG msg;

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
   static int  yClient;
   static int  scrollPos;
   HDC         dc;
   int         y;
   PAINTSTRUCT ps;
   WCHAR       buffer[ 10 ] = { 0 };
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

      SetScrollRange(wnd, SB_VERT, 0, NUMLINES - 1, FALSE);
      SetScrollPos(wnd, SB_VERT, scrollPos, TRUE);
      return 0;

   case WM_SIZE:
      yClient = GET_Y_LPARAM(lParam);
      return 0;

   case WM_VSCROLL:
      switch ( LOWORD(wParam) )
      {
      case SB_LINEUP:
         scrollPos -= 1;
         break;

      case SB_LINEDOWN:
         scrollPos += 1;
         break;

      case SB_PAGEUP:
         scrollPos -= yClient / yChar;
         break;

      case SB_PAGEDOWN:
         scrollPos += yClient / yChar;
         break;

      case SB_THUMBPOSITION:
         scrollPos = HIWORD(wParam);
         break;

      default:
         break;
      }

      scrollPos = max(0, min(scrollPos, NUMLINES - 1));

      if ( scrollPos != GetScrollPos(wnd, SB_VERT) )
      {
         SetScrollPos(wnd, SB_VERT, scrollPos, TRUE);
         InvalidateRect(wnd, NULL, TRUE);
      }
      return 0;

   case WM_PAINT:
      dc = BeginPaint(wnd, &ps);

      for ( unsigned i = 0; i < NUMLINES; i++ )
      {
         y = yChar * (i - scrollPos);

         TextOutW(dc, 1, y,
                  sysmetrics[ i ].label,
                  lstrlenW(sysmetrics[ i ].label));

         TextOutW(dc, 22 * xCaps, y + 1,
                  sysmetrics[ i ].desc,
                  lstrlenW(sysmetrics[ i ].desc));

         SetTextAlign(dc, TA_RIGHT | TA_TOP);

         TextOutW(dc, 22 * xCaps + 40 * xChar + 1, y, buffer,
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