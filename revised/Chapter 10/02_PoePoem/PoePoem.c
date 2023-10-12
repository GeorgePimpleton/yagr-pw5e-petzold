/*-------------------------------------------
   POEPOEM.C -- Demonstrates Custom Resource
                (c) Charles Petzold, 1998
  -------------------------------------------*/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>

#include "Resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE g_inst;

int WINAPI wWinMain(_In_     HINSTANCE inst,
                    _In_opt_ HINSTANCE prevInst,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInst);
   UNREFERENCED_PARAMETER(cmdLine);

   WCHAR     appName[ 16 ];
   WCHAR     caption[ 64 ];
   WCHAR     errMsg[ 64 ];
   HWND      wnd;
   MSG       msg;
   WNDCLASSW wc;

   LoadStringW(inst, IDS_APPNAME, appName, _countof(appName));

   LoadStringW(inst, IDS_CAPTION, caption, _countof(caption));

   wc.style         = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = inst;
   wc.hIcon         = (HICON)   LoadImageW(inst, appName, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wc.hbrBackground = (HBRUSH)  (COLOR_WINDOW + 1);
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = appName;

   g_inst = inst;

   if ( !RegisterClassW(&wc) )
   {
      LoadStringW(inst, IDS_APPNAME, (WCHAR*) appName, _countof(appName));

      LoadStringW(inst, IDS_ERRMSG, (WCHAR*) errMsg, _countof(errMsg));

      MessageBoxW(NULL, (WCHAR*) errMsg, (WCHAR*) appName, MB_ICONERROR);
      return 0;
   }

   wnd = CreateWindowW(appName, caption,
                       WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
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
   static char*   text;
   static char*   rsc;
   static char*   working;
   static DWORD   fileSize = 0;
   static HGLOBAL resource;
   static HWND    scroll;
   static int     position;
   static int     xChar;
   static int     yChar;
   static int     yClient;
   static int     numLines;
   static int     xScroll;
   HDC            dc;
   PAINTSTRUCT    ps;
   RECT           rect;
   TEXTMETRICW    tm;

   switch ( msg )
   {
   case WM_CREATE:
      dc = GetDC(wnd);
      GetTextMetricsW(dc, &tm);
      xChar = tm.tmAveCharWidth;
      yChar = tm.tmHeight + tm.tmExternalLeading;
      ReleaseDC(wnd, dc);

      xScroll = GetSystemMetrics(SM_CXVSCROLL);

      scroll = CreateWindowW(L"scrollbar", NULL,
                             WS_CHILD | WS_VISIBLE | SBS_VERT,
                             0, 0, 0, 0,
                             wnd, (HMENU) 1, g_inst, NULL);

      HRSRC rc = FindResourceW(g_inst, L"ANNABELLEE", L"TEXT");

      if ( rc )
      {
         resource = LoadResource(g_inst, rc);
         fileSize = SizeofResource(g_inst, rc);

         if ( resource ) text = (char*) LockResource(resource);
      }
      rsc = (char*) malloc(sizeof(char) * fileSize) ;

      if ( rsc ) memcpy(rsc, text, fileSize) ;
      working = rsc;

      /*hResource = LoadResource(
         hInst,
         FindResource(hInst, L"AnnabelLee", L"TEXT"));*/

         //pText = (char*)LockResource(hResource);

      numLines = 0;

      if ( working )
      {
         while ( *working != '\\' && *working != '\0' )
         {
            if ( *working == '\n' )
            {
               numLines++;
            }
            working = AnsiNext(working);
         }
         *working = '\0';
      }

      SetScrollRange(scroll, SB_CTL, 0, numLines, FALSE);
      SetScrollPos(scroll, SB_CTL, 0, FALSE);
      return 0;

   case WM_SIZE:
      MoveWindow(scroll, GET_X_LPARAM(lParam) - xScroll, 0,
                 xScroll, yClient = GET_Y_LPARAM(lParam), TRUE);
      SetFocus(wnd);
      return 0;

   case WM_SETFOCUS:
      SetFocus(scroll);
      return 0;

   case WM_VSCROLL:
      switch ( wParam )
      {
      case SB_TOP:
         position = 0;
         break;

      case SB_BOTTOM:
         position = numLines;
         break;

      case SB_LINEUP:
         position -= 1;
         break;

      case SB_LINEDOWN:
         position += 1;
         break;

      case SB_PAGEUP:
         position -= yClient / yChar;
         break;

      case SB_PAGEDOWN:
         position += yClient / yChar;
         break;

      case SB_THUMBPOSITION:
         position = LOWORD(lParam);
         break;
      }
      position = max(0, min(position, numLines));

      if ( position != GetScrollPos(scroll, SB_CTL) )
      {
         SetScrollPos(scroll, SB_CTL, position, TRUE);
         InvalidateRect(wnd, NULL, TRUE);
      }
      return 0;

   case WM_PAINT:
      dc = BeginPaint(wnd, &ps);

      //pText = (char*)LockResource(hResource);

      GetClientRect(wnd, &rect);
      rect.left += xChar;
      rect.top  += yChar * (1 - position);
      DrawTextA(dc, rsc, -1, &rect, DT_EXTERNALLEADING);

      EndPaint(wnd, &ps);
      return 0;

   case WM_DESTROY:
      FreeResource(resource);
      free(rsc);
      PostQuitMessage(0);
      return 0;
   }
   return DefWindowProcW(wnd, msg, wParam, lParam);
}
