/*------------------------------------------
   ABOUT3.C -- About Box Demo Program No. 3
               (c) Charles Petzold, 1998
  ------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include "Resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK EllipPushWndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(_In_     HINSTANCE inst,
                    _In_opt_ HINSTANCE prevInst,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInst);
   UNREFERENCED_PARAMETER(cmdLine);

   static WCHAR appName[ ] = L"About3";
   MSG          msg;
   HWND         wnd;
   WNDCLASS     wc;

   wc.style         = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = inst;
   wc.hIcon         = (HICON)   LoadImageW(inst, appName, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wc.hbrBackground = (HBRUSH)  (COLOR_WINDOW + 1);
   wc.lpszMenuName  = appName;
   wc.lpszClassName = appName;

   if ( !RegisterClassW(&wc) )
   {
      MessageBoxW(NULL, L"This program requires Windows NT!", appName, MB_ICONERROR);
      return 0;
   }

   wc.style         = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = EllipPushWndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = inst;
   wc.hIcon         = NULL;
   wc.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wc.hbrBackground = (HBRUSH)  (COLOR_BTNFACE + 1);
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = L"EllipPush";

   RegisterClassW(&wc);

   wnd = CreateWindowW(appName, L"About Box Demo Program",
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
   static HINSTANCE inst;

   switch ( msg )
   {
   case WM_CREATE:
      inst = GetWindowInstance(wnd);
      return 0;

   case WM_COMMAND:
      switch ( LOWORD(wParam) )
      {
      case IDM_APP_ABOUT:
         DialogBoxParamW(inst, L"AboutBox", wnd, AboutDlgProc, 0L);
         return 0;
      }
      break;

   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   }
   return DefWindowProcW(wnd, msg, wParam, lParam);
}

INT_PTR CALLBACK AboutDlgProc(HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch ( msg )
   {
   case WM_INITDIALOG:
      return TRUE;

   case WM_COMMAND:
      switch ( LOWORD(wParam) )
      {
      case IDOK:
         EndDialog(dlg, 0);
         return TRUE;
      }
      break;
   }
   return FALSE;
}

LRESULT CALLBACK EllipPushWndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   WCHAR       text[ 40 ];
   HBRUSH      brush;
   HDC         dc;
   PAINTSTRUCT ps;
   RECT        rect;

   switch ( msg )
   {
   case WM_PAINT:
      GetClientRect(wnd, &rect);
      GetWindowTextW(wnd, text, sizeof(text));

      dc = BeginPaint(wnd, &ps);

      brush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
      brush = (HBRUSH) SelectObject(dc, brush);
      SetBkColor(dc, GetSysColor(COLOR_WINDOW));
      SetTextColor(dc, GetSysColor(COLOR_WINDOWTEXT));

      Ellipse(dc, rect.left, rect.top, rect.right, rect.bottom);
      DrawTextW(dc, text, -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

      DeleteObject(SelectObject(dc, brush));

      EndPaint(wnd, &ps);
      return 0;

   case WM_KEYUP:
      if ( wParam != VK_SPACE )
         break;
      // fall through

   case WM_LBUTTONUP:
      SendMessageW(GetParent(wnd), WM_COMMAND, GetWindowLongPtrW(wnd, GWLP_ID), (LPARAM) wnd);
      return 0;
   }
   return DefWindowProcW(wnd, msg, wParam, lParam);
}
