/*-----------------------------------------------------------
   PRINT.C -- Common routines for Print1, Print2, and Print3
  -----------------------------------------------------------*/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include "Print.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

extern HINSTANCE xInst;
extern PCWSTR    xAppName;
extern PCWSTR    xCaption;

int WINAPI wWinMain(_In_     HINSTANCE inst,
                    _In_opt_ HINSTANCE prevInst,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInst);
   UNREFERENCED_PARAMETER(cmdLine);

   HWND     wnd;
   MSG      msg;
   WNDCLASS wc = { 0 };

   wc.style         = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = inst;
   wc.hIcon         = (HICON)   LoadImageW(inst, xAppName, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wc.hbrBackground = (HBRUSH)  (COLOR_WINDOW + 1);
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = xAppName;

   if ( !RegisterClassW(&wc) )
   {
      MessageBoxW(NULL, L"This program requires Windows NT!", xAppName, MB_ICONERROR);
      return 0;
   }

   xInst = inst;

   wnd = CreateWindowW(xAppName, xCaption,
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

void PageGDICalls(HDC dcPrn, int xPage, int yPage)
{
   static PCWSTR textStr = L"Hello, Printer!";

   Rectangle(dcPrn, 0, 0, xPage, yPage);

   MoveToEx(dcPrn, 0, 0, NULL);
   LineTo(dcPrn, xPage, yPage);
   MoveToEx(dcPrn, xPage, 0, NULL);
   LineTo(dcPrn, 0, yPage);

   SaveDC(dcPrn);

   SetMapMode(dcPrn, MM_ISOTROPIC);
   SetWindowExtEx(dcPrn, 1000, 1000, NULL);
   SetViewportExtEx(dcPrn, xPage / 2, -yPage / 2, NULL);
   SetViewportOrgEx(dcPrn, xPage / 2, yPage / 2, NULL);

   Ellipse(dcPrn, -500, 500, 500, -500);

   SetTextAlign(dcPrn, TA_BASELINE | TA_CENTER);
   TextOutW(dcPrn, 0, 0, textStr, lstrlenW(textStr));

   RestoreDC(dcPrn, -1);
}

LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static int  xClient;
   static int  yClient;
   HDC         dc;
   HMENU       menu;
   PAINTSTRUCT ps;

   switch ( msg )
   {
   case WM_CREATE:
      menu = GetSystemMenu(wnd, FALSE);
      AppendMenuW(menu, MF_SEPARATOR, 0, NULL);
      AppendMenuW(menu, 0, 1, TEXT("&Print"));
      return 0;

   case WM_SIZE:
      xClient = GET_X_LPARAM(lParam);
      yClient = GET_Y_LPARAM(lParam);
      return 0;

   case WM_SYSCOMMAND:
      if ( wParam == 1 )
      {
         if ( !PrintMyPage(wnd) )
         {
            MessageBoxW(wnd, L"Could not print page!", xAppName, MB_OK | MB_ICONEXCLAMATION);
         }
         return 0;
      }
      break;

   case WM_PAINT:
      dc = BeginPaint(wnd, &ps);

      PageGDICalls(dc, xClient, yClient);

      EndPaint(wnd, &ps);
      return 0;

   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   }
   return DefWindowProcW(wnd, msg, wParam, lParam);
}