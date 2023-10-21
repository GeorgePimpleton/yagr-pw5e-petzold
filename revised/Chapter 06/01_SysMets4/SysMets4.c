/*----------------------------------------------------
   SYSMETS4.C -- System Metrics Display Program No. 4
                 (c) Charles Petzold, 1998
  ----------------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>

#include "SysMets.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(_In_     HINSTANCE inst,
                    _In_opt_ HINSTANCE prevInst,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInst);
   UNREFERENCED_PARAMETER(cmdLine);

   static PCWSTR appName = L"SysMets4";
   HWND          wnd;
   MSG           msg;
   WNDCLASSW     wc      = { 0 };

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
      MessageBoxW(NULL, L"Program requires Windows NT!",
                  appName, MB_ICONERROR);
      return 0;
   }

   wnd = CreateWindowW(appName, L"Get System Metrics No. 4",
                       WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL,
                       CW_USEDEFAULT, CW_USEDEFAULT,
                       CW_USEDEFAULT, CW_USEDEFAULT,
                       NULL, NULL, inst, NULL);

   ShowWindow(wnd, showCmd);
   UpdateWindow(wnd);

   while ( GetMessage(&msg, NULL, 0, 0) )
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
   static int  xClient;
   static int  yClient;
   static int  maxWidth;
   HDC         dc;
   int         i;
   int         x;
   int         y;
   int         vertPos;
   int         horzPos;
   int         paintBeg;
   int         paintEnd;
   PAINTSTRUCT ps;
   SCROLLINFO si           = { 0 };
   WCHAR      buffer[ 10 ] = { L"" };
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

      // save the width of the three columns
      maxWidth = 40 * xChar + 22 * xCaps;
      return 0;

   case WM_SIZE:
      xClient = GET_X_LPARAM(lParam);
      yClient = GET_Y_LPARAM(lParam);

      // set vertical scroll bar range and page size
      si.cbSize = sizeof(si);
      si.fMask  = SIF_RANGE | SIF_PAGE;
      si.nMin   = 0;
      si.nMax   = NUMLINES - 1;
      si.nPage  = yClient / yChar;
      SetScrollInfo(wnd, SB_VERT, &si, TRUE);

      // set horizontal scroll bar range and page size
      si.cbSize = sizeof(si);
      si.fMask  = SIF_RANGE | SIF_PAGE;
      si.nMin   = 0;
      si.nMax   = 2 + maxWidth / xChar;
      si.nPage  = xClient / xChar;
      SetScrollInfo(wnd, SB_HORZ, &si, TRUE);
      return 0;

   case WM_VSCROLL:
      // get all the vertical scroll bar information
      si.cbSize = sizeof(si);
      si.fMask  = SIF_ALL;
      GetScrollInfo(wnd, SB_VERT, &si);

      // save the position for comparison later on
      vertPos = si.nPos;

      switch ( LOWORD(wParam) )
      {
      case SB_TOP:
         si.nPos = si.nMin;
         break;

      case SB_BOTTOM:
         si.nPos = si.nMax;
         break;

      case SB_LINEUP:
         si.nPos -= 1;
         break;

      case SB_LINEDOWN:
         si.nPos += 1;
         break;

      case SB_PAGEUP:
         si.nPos -= si.nPage;
         break;

      case SB_PAGEDOWN:
         si.nPos += si.nPage;
         break;

      case SB_THUMBTRACK:
         si.nPos = si.nTrackPos;
         break;

      default:
         break;
      }
      // set the position and then retrieve it.  due to adjustments
      //   by Windows it might not be the same as the value set.

      si.fMask = SIF_POS;
      SetScrollInfo(wnd, SB_VERT, &si, TRUE);
      GetScrollInfo(wnd, SB_VERT, &si);

      // if the position has changed, scroll the window and update it
      if ( si.nPos != vertPos )
      {
         ScrollWindow(wnd, 0, yChar * (vertPos - si.nPos),
                      NULL, NULL);
         UpdateWindow(wnd);
      }
      return 0;

   case WM_HSCROLL:
      // get all the vertical scroll bar information
      si.cbSize = sizeof(si);
      si.fMask = SIF_ALL;

      // save the position for comparison later on
      GetScrollInfo(wnd, SB_HORZ, &si);
      horzPos = si.nPos;

      switch ( LOWORD(wParam) )
      {
      case SB_LINELEFT:
         si.nPos -= 1;
         break;

      case SB_LINERIGHT:
         si.nPos += 1;
         break;

      case SB_PAGELEFT:
         si.nPos -= si.nPage;
         break;

      case SB_PAGERIGHT:
         si.nPos += si.nPage;
         break;

      case SB_THUMBPOSITION:
         si.nPos = si.nTrackPos;
         break;

      default:
         break;
      }

      // set the position and then retrieve it.  due to adjustments
      //   by Windows it might not be the same as the value set.
      si.fMask = SIF_POS;
      SetScrollInfo(wnd, SB_HORZ, &si, TRUE);
      GetScrollInfo(wnd, SB_HORZ, &si);

      // if the position has changed, scroll the window
      if ( si.nPos != horzPos )
      {
         ScrollWindow(wnd, xChar * (horzPos - si.nPos), 0, NULL, NULL);
      }
      return 0;

   case WM_KEYDOWN:
      switch ( wParam )
      {
      case VK_HOME:
         SendMessageW(wnd, WM_VSCROLL, SB_TOP, 0);
         break;

      case VK_END:
         SendMessageW(wnd, WM_VSCROLL, SB_BOTTOM, 0);
         break;

      case VK_PRIOR:
         SendMessageW(wnd, WM_VSCROLL, SB_PAGEUP, 0);
         break;

      case VK_NEXT:
         SendMessageW(wnd, WM_VSCROLL, SB_PAGEDOWN, 0);
         break;

      case VK_UP:
         SendMessageW(wnd, WM_VSCROLL, SB_LINEUP, 0);
         break;

      case VK_DOWN:
         SendMessageW(wnd, WM_VSCROLL, SB_LINEDOWN, 0);
         break;

      case VK_LEFT:
         SendMessageW(wnd, WM_HSCROLL, SB_PAGEUP, 0);
         break;

      case VK_RIGHT:
         SendMessageW(wnd, WM_HSCROLL, SB_PAGEDOWN, 0);
         break;
      }
      return 0;

   case WM_PAINT:
      dc = BeginPaint(wnd, &ps);

      // get vertical scroll bar position
      si.cbSize = sizeof(si);
      si.fMask  = SIF_POS;
      GetScrollInfo(wnd, SB_VERT, &si);
      vertPos = si.nPos;

      // get horizontal scroll bar position
      GetScrollInfo(wnd, SB_HORZ, &si);
      horzPos = si.nPos;

      // find painting limits
      paintBeg = max(0, vertPos + ps.rcPaint.top / yChar);
      paintEnd = min(NUMLINES - 1,
                     vertPos + ps.rcPaint.bottom / yChar);

      for ( i = paintBeg; i <= paintEnd; i++ )
      {
         x = xChar * (1 - horzPos);
         y = yChar * (i - vertPos);

         TextOutW(dc, x, y,
                  sysmetrics[ i ].label,
                  lstrlenW(sysmetrics[ i ].label));

         TextOutW(dc, x + 22 * xCaps, y,
                  sysmetrics[ i ].desc,
                  lstrlenW(sysmetrics[ i ].desc));

         SetTextAlign(dc, TA_RIGHT | TA_TOP);

         TextOutW(dc, x + 22 * xCaps + 40 * xChar, y, buffer,
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