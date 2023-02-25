/*----------------------------------------------------
   SYSMETS3.C -- System Metrics Display Program No. 3
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

   static WCHAR szAppName[] = L"SysMets3";
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
      MessageBoxW(NULL, L"Program requires Windows NT!",
                  szAppName, MB_ICONERROR);
      return 0;
   }

   hwnd = CreateWindowW(szAppName, L"Get System Metrics No. 3",
                        WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL,
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
   static int  cxClient;
   static int  cyClient;
   static int  iMaxWidth;
   HDC         hdc;
   int         i;
   int         x;
   int         y;
   int         iVertPos;
   int         iHorzPos;
   int         iPaintBeg;
   int         iPaintEnd;
   PAINTSTRUCT ps;
   SCROLLINFO  si;
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

      // Save the width of the three columns
      iMaxWidth = 40 * cxChar + 22 * cxCaps;
      return 0;

   case WM_SIZE:
      cxClient = GET_X_LPARAM(lParam);
      cyClient = GET_Y_LPARAM(lParam);

      // Set vertical scroll bar range and page size

      si.cbSize = sizeof(si);
      si.fMask  = SIF_RANGE | SIF_PAGE;
      si.nMin   = 0;
      si.nMax   = NUMLINES - 1;
      si.nPage  = cyClient / cyChar;
      SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

      // Set horizontal scroll bar range and page size
      si.cbSize = sizeof(si);
      si.fMask  = SIF_RANGE | SIF_PAGE;
      si.nMin   = 0;
      si.nMax   = 2 + iMaxWidth / cxChar;
      si.nPage  = cxClient / cxChar;
      SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
      return 0;

   case WM_VSCROLL:
      // Get all the vertial scroll bar information
      si.cbSize = sizeof(si);
      si.fMask = SIF_ALL;
      GetScrollInfo(hwnd, SB_VERT, &si);

      // Save the position for comparison later on
      iVertPos = si.nPos;

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
      // Set the position and then retrieve it.  Due to adjustments
      //   by Windows it may not be the same as the value set.

      si.fMask = SIF_POS;
      SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
      GetScrollInfo(hwnd, SB_VERT, &si);

      // If the position has changed, scroll the window and update it
      if ( si.nPos != iVertPos )
      {
         ScrollWindow(hwnd, 0, cyChar * (iVertPos - si.nPos), NULL, NULL);
         UpdateWindow(hwnd);
      }
      return 0;

   case WM_HSCROLL:
      // Get all the vertial scroll bar information
      si.cbSize = sizeof(si);
      si.fMask  = SIF_ALL;

      // Save the position for comparison later on
      GetScrollInfo(hwnd, SB_HORZ, &si);
      iHorzPos = si.nPos;

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
      // Set the position and then retrieve it.  Due to adjustments
      //   by Windows it may not be the same as the value set.
      si.fMask = SIF_POS;
      SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
      GetScrollInfo(hwnd, SB_HORZ, &si);

      // If the position has changed, scroll the window

      if ( si.nPos != iHorzPos )
      {
         ScrollWindow(hwnd, cxChar * (iHorzPos - si.nPos), 0,
                      NULL, NULL);
      }
      return 0;

   case WM_PAINT:
      hdc = BeginPaint(hwnd, &ps);

      // Get vertical scroll bar position
      si.cbSize = sizeof(si);
      si.fMask  = SIF_POS;
      GetScrollInfo(hwnd, SB_VERT, &si);
      iVertPos  = si.nPos;

      // Get horizontal scroll bar position
      GetScrollInfo(hwnd, SB_HORZ, &si);
      iHorzPos = si.nPos;

      // Find painting limits
      iPaintBeg = max(0, iVertPos + ps.rcPaint.top / cyChar);
      iPaintEnd = min(NUMLINES - 1,
                      iVertPos + ps.rcPaint.bottom / cyChar);

      for ( i = iPaintBeg; i <= iPaintEnd; i++ )
      {
         x = cxChar * (1 - iHorzPos);
         y = cyChar * (i - iVertPos);

         TextOutW(hdc, x, y,
                  sysmetrics[ i ].szLabel,
                  lstrlenW(sysmetrics[ i ].szLabel));

         TextOutW(hdc, x + 22 * cxCaps, y,
                  sysmetrics[ i ].szDesc,
                  lstrlenW(sysmetrics[ i ].szDesc));

         SetTextAlign(hdc, TA_RIGHT | TA_TOP);

         TextOutW(hdc, x + 22 * cxCaps + 40 * cxChar, y, szBuffer,
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