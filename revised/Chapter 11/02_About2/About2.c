/*------------------------------------------
   ABOUT2.C -- About Box Demo Program No. 2
               (c) Charles Petzold, 1998
  ------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include "Resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);

int currentColor  = IDC_BLACK;
int currentFigure = IDC_RECT;

int WINAPI wWinMain(_In_     HINSTANCE inst,
                    _In_opt_ HINSTANCE prevInst,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInst);
   UNREFERENCED_PARAMETER(cmdLine);

   static WCHAR appName[ ] = L"About2";
   MSG          msg;
   HWND         wnd;
   WNDCLASSW    wc;

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

void PaintWindow(HWND wnd, int color, int figure)
{
   static COLORREF crColor[ 8 ] = { RGB(0,   0, 0), RGB(0,   0, 255),
                                    RGB(0, 255, 0), RGB(0, 255, 255),
                                    RGB(255,   0, 0), RGB(255,   0, 255),
                                    RGB(255, 255, 0), RGB(255, 255, 255) };

   HBRUSH brush;
   HDC    dc;
   RECT   rect;

   dc = GetDC(wnd);
   GetClientRect(wnd, &rect);
   brush = CreateSolidBrush(crColor[ color - IDC_BLACK ]);
   brush = (HBRUSH) SelectObject(dc, brush);

   if ( figure == IDC_RECT )
   {
      Rectangle(dc, rect.left, rect.top, rect.right, rect.bottom);
   }
   else
   {
      Ellipse(dc, rect.left, rect.top, rect.right, rect.bottom);
   }

   DeleteObject(SelectObject(dc, brush));
   ReleaseDC(wnd, dc);
}

void PaintTheBlock(HWND ctrl, int color, int figure)
{
   InvalidateRect(ctrl, NULL, TRUE);
   UpdateWindow(ctrl);
   PaintWindow(ctrl, color, figure);
}

LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static HINSTANCE inst;
   PAINTSTRUCT      ps;

   switch ( msg )
   {
   case WM_CREATE:
      inst = GetWindowInstance(wnd);
      return 0;

   case WM_COMMAND:
      switch ( LOWORD(wParam) )
      {
      case IDM_APP_ABOUT:
         if ( DialogBoxParamW(inst, L"AboutBox", wnd, AboutDlgProc, 0L) )
         {
            InvalidateRect(wnd, NULL, TRUE);
         }
         return 0;
      }
      break;

   case WM_PAINT:
      BeginPaint(wnd, &ps);
      EndPaint(wnd, &ps);

      PaintWindow(wnd, currentColor, currentFigure);
      return 0;

   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   }
   return DefWindowProcW(wnd, msg, wParam, lParam);
}

INT_PTR CALLBACK AboutDlgProc(HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static HWND ctrlBlock;
   static int  color;
   static int  figure;

   switch ( msg )
   {
   case WM_INITDIALOG:
      color  = currentColor;
      figure = currentFigure;

      CheckRadioButton(dlg, IDC_BLACK, IDC_WHITE, color);
      CheckRadioButton(dlg, IDC_RECT, IDC_ELLIPSE, figure);

      ctrlBlock = GetDlgItem(dlg, IDC_PAINT);

      SetFocus(GetDlgItem(dlg, color));
      return FALSE;

   case WM_COMMAND:
      switch ( LOWORD(wParam) )
      {
      case IDOK:
         currentColor  = color;
         currentFigure = figure;
         EndDialog(dlg, TRUE);
         return TRUE;

      case IDCANCEL:
         EndDialog(dlg, FALSE);
         return TRUE;

      case IDC_BLACK:
      case IDC_RED:
      case IDC_GREEN:
      case IDC_YELLOW:
      case IDC_BLUE:
      case IDC_MAGENTA:
      case IDC_CYAN:
      case IDC_WHITE:
         color = LOWORD(wParam);
         CheckRadioButton(dlg, IDC_BLACK, IDC_WHITE, LOWORD(wParam));
         PaintTheBlock(ctrlBlock, color, figure);
         return TRUE;

      case IDC_RECT:
      case IDC_ELLIPSE:
         figure = LOWORD(wParam);
         CheckRadioButton(dlg, IDC_RECT, IDC_ELLIPSE, LOWORD(wParam));
         PaintTheBlock(ctrlBlock, color, figure);
         return TRUE;
      }
      break;

   case WM_PAINT:
      PaintTheBlock(ctrlBlock, color, figure);
      break;
   }
   return FALSE;
}