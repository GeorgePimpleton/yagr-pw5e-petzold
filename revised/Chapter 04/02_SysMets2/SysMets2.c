/*----------------------------------------------------
   SYSMETS2.C -- System Metrics Display Program No. 2
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

   static PCWSTR appName = L"SysMets2";
   static PCWSTR appTitle = L"Get System Metrics No. 2";
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

   HWND hwnd = CreateWindowW(appName, appTitle,
                             WS_OVERLAPPEDWINDOW | WS_VSCROLL,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             NULL, NULL, instance, NULL);

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

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   static int  cxChar;
   static int  cxCaps;
   static int  cyChar;
   static int  cyClient;
   static int  vScrollPos;
   HDC         hdc;
   int         y;
   PAINTSTRUCT ps;
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

      SetScrollRange(hwnd, SB_VERT, 0, NUMLINES - 1, FALSE);
      SetScrollPos(hwnd, SB_VERT, vScrollPos, TRUE);
      return 0;

   case WM_SIZE:
      cyClient = GET_Y_LPARAM(lParam);
      return 0;

   case WM_VSCROLL:
      switch ( LOWORD(wParam) )
      {
      case SB_LINEUP:
         vScrollPos -= 1;
         break;

      case SB_LINEDOWN:
         vScrollPos += 1;
         break;

      case SB_PAGEUP:
         vScrollPos -= cyClient / cyChar;
         break;

      case SB_PAGEDOWN:
         vScrollPos += cyClient / cyChar;
         break;

      case SB_THUMBPOSITION:
         vScrollPos = HIWORD(wParam);
         break;

      default:
         break;
      }

      vScrollPos = max(0, min(vScrollPos, NUMLINES - 1));

      if ( vScrollPos != GetScrollPos(hwnd, SB_VERT) )
      {
         SetScrollPos(hwnd, SB_VERT, vScrollPos, TRUE);
         InvalidateRect(hwnd, NULL, TRUE);
      }
      return 0;

   case WM_PAINT:
      hdc = BeginPaint(hwnd, &ps);

      for ( unsigned i = 0; i < NUMLINES; i++ )
      {
         y = cyChar * (i - vScrollPos);

         TextOutW(hdc, 1, y,
                  sysmetrics[ i ].szLabel,
                  lstrlenW(sysmetrics[ i ].szLabel));

         TextOutW(hdc, 22 * cxCaps, y + 1,
                  sysmetrics[ i ].szDesc,
                  lstrlenW(sysmetrics[ i ].szDesc));

         SetTextAlign(hdc, TA_RIGHT | TA_TOP);

         TextOutW(hdc, 22 * cxCaps + 40 * cxChar + 1, y, buffer,
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