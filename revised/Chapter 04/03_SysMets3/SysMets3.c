/*----------------------------------------------------
   SYSMETS3.C -- System Metrics Display Program No. 3
             (c) Charles Petzold, 1998
  ----------------------------------------------------*/

#define WIN32_MEAN_AND_LEAN

#include <windows.h>
#include <windowsx.h>

#include "SysMets.h"  // from 01_SysMets1

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(_In_     HINSTANCE instance,
                    _In_opt_ HINSTANCE prevInstance,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInstance);
   UNREFERENCED_PARAMETER(cmdLine);

   static PCWSTR appName  = L"SysMets3";
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
      MessageBoxW(NULL, L"Program requires Windows NT!",
                  appName, MB_ICONERROR);
      return 0;
   }

   hwnd = CreateWindowW(appName, L"Get System Metrics No. 3",
                        WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL,
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
   static int  cxClient;
   static int  cyClient;
   static int  maxWidth;
   HDC         hdc;
   int         i;
   int         x;
   int         y;
   int         vertPos;
   int         horzPos;
   int         paintBeg;
   int         paintEnd;
   PAINTSTRUCT ps;
   SCROLLINFO  si;
   WCHAR       buffer[ 10 ];
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

      // save the width of the three columns
      maxWidth = 40 * cxChar + 22 * cxCaps;
      return 0;

   case WM_SIZE:
      cxClient = GET_X_LPARAM(lParam);
      cyClient = GET_Y_LPARAM(lParam);

      // set vertical scroll bar range and page size
      si.cbSize = sizeof(si);
      si.fMask  = SIF_RANGE | SIF_PAGE;
      si.nMin   = 0;
      si.nMax   = NUMLINES - 1;
      si.nPage  = cyClient / cyChar;
      SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

      // set horizontal scroll bar range and page size
      si.cbSize = sizeof(si);
      si.fMask  = SIF_RANGE | SIF_PAGE;
      si.nMin   = 0;
      si.nMax   = 2 + maxWidth / cxChar;
      si.nPage  = cxClient / cxChar;
      SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
      return 0;

   case WM_VSCROLL:
      // get all the vertial scroll bar information
      si.cbSize = sizeof(si);
      si.fMask  = SIF_ALL;
      GetScrollInfo(hwnd, SB_VERT, &si);

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
      //   by Windows it may not be the same as the value set.

      si.fMask = SIF_POS;
      SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
      GetScrollInfo(hwnd, SB_VERT, &si);

      // if the position has changed, scroll the window and update it
      if ( si.nPos != vertPos )
      {
         ScrollWindow(hwnd, 0, cyChar * (vertPos - si.nPos), NULL, NULL);
         UpdateWindow(hwnd);
      }
      return 0;

   case WM_HSCROLL:
      // get all the vertial scroll bar information
      si.cbSize = sizeof(si);
      si.fMask  = SIF_ALL;

      // save the position for comparison later on
      GetScrollInfo(hwnd, SB_HORZ, &si);
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
      //   by Windows it may not be the same as the value set.
      si.fMask = SIF_POS;
      SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
      GetScrollInfo(hwnd, SB_HORZ, &si);

      // If the position has changed, scroll the window

      if ( si.nPos != horzPos )
      {
         ScrollWindow(hwnd, cxChar * (horzPos - si.nPos), 0,
                      NULL, NULL);
      }
      return 0;

   case WM_PAINT:
      hdc = BeginPaint(hwnd, &ps);

      // get vertical scroll bar position
      si.cbSize = sizeof(si);
      si.fMask  = SIF_POS;
      GetScrollInfo(hwnd, SB_VERT, &si);
      vertPos  = si.nPos;

      // get horizontal scroll bar position
      GetScrollInfo(hwnd, SB_HORZ, &si);
      horzPos = si.nPos;

      // find painting limits
      paintBeg = max(0, vertPos + ps.rcPaint.top / cyChar);
      paintEnd = min(NUMLINES - 1,
                      vertPos + ps.rcPaint.bottom / cyChar);

      for ( i = paintBeg; i <= paintEnd; i++ )
      {
         x = cxChar * (1 - horzPos);
         y = cyChar * (i - vertPos);

         TextOutW(hdc, x, y,
                  sysmetrics[ i ].szLabel,
                  lstrlenW(sysmetrics[ i ].szLabel));

         TextOutW(hdc, x + 22 * cxCaps, y,
                  sysmetrics[ i ].szDesc,
                  lstrlenW(sysmetrics[ i ].szDesc));

         SetTextAlign(hdc, TA_RIGHT | TA_TOP);

         TextOutW(hdc, x + 22 * cxCaps + 40 * cxChar, y, buffer,
                  wsprintfW(buffer, L"%5d",
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