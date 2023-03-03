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

HINSTANCE inst;

int WINAPI wWinMain(_In_     HINSTANCE instance,
                    _In_opt_ HINSTANCE prevInstance,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInstance);
   UNREFERENCED_PARAMETER(cmdLine);

   WCHAR       appName[ 16 ];
   WCHAR       caption[ 64 ];
   WCHAR       errMsg[ 64 ];
   HWND        hwnd;
   MSG         msg;
   WNDCLASSW   wc;

   LoadStringW(instance, IDS_APPNAME, appName, _countof(appName));

   LoadStringW(instance, IDS_CAPTION, caption, _countof(caption));

   wc.style         = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = instance;
   wc.hIcon         = (HICON)   LoadImageW(instance, appName, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wc.hbrBackground = (HBRUSH)  (COLOR_WINDOW + 1);
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = appName;

   inst = instance;

   if ( !RegisterClassW(&wc) )
   {
      LoadStringW(instance, IDS_APPNAME, (WCHAR*) appName, _countof(appName));

      LoadStringW(instance, IDS_ERRMSG, (WCHAR*) errMsg, _countof(errMsg));

      MessageBoxW(NULL, (WCHAR*) errMsg, (WCHAR*) appName, MB_ICONERROR);
      return 0;
   }

   hwnd = CreateWindowW(appName, caption,
                        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
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
   static char*   text;
   static char*   rsc;
   static char*   working;
   static DWORD   fileSize = 0;
   static HGLOBAL resource;
   static HWND    scroll;
   static int     position;
   static int     cxChar;
   static int     cyChar;
   static int     cyClient;
   static int     numLines;
   static int     xScroll;
   HDC            hdc;
   PAINTSTRUCT    ps;
   RECT           rect;
   TEXTMETRICW    tm;

   switch ( message )
   {
   case WM_CREATE:
      hdc = GetDC(hwnd);
      GetTextMetricsW(hdc, &tm);
      cxChar = tm.tmAveCharWidth;
      cyChar = tm.tmHeight + tm.tmExternalLeading;
      ReleaseDC(hwnd, hdc);

      xScroll = GetSystemMetrics(SM_CXVSCROLL);

      scroll = CreateWindowW(L"scrollbar", NULL,
                             WS_CHILD | WS_VISIBLE | SBS_VERT,
                             0, 0, 0, 0,
                             hwnd, (HMENU) 1, inst, NULL);

      HRSRC rc = FindResourceW(inst, L"ANNABELLEE", L"TEXT");
      if ( rc )
      {
         resource = LoadResource(inst, rc);
         fileSize = SizeofResource(inst, rc);
         if ( resource ) text = (char*) LockResource(resource);
      }
      rsc = (char*) malloc(sizeof(char) * fileSize);
      if ( rsc ) memcpy(rsc, text, fileSize);
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
               numLines++;
            working = AnsiNext(working);
         }
         *working = '\0';
      }

      SetScrollRange(scroll, SB_CTL, 0, numLines, FALSE);
      SetScrollPos(scroll, SB_CTL, 0, FALSE);
      return 0;

   case WM_SIZE:
      MoveWindow(scroll, GET_X_LPARAM(lParam) - xScroll, 0,
                 xScroll, cyClient = GET_Y_LPARAM(lParam), TRUE);
      SetFocus(hwnd);
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
         position -= cyClient / cyChar;
         break;
      case SB_PAGEDOWN:
         position += cyClient / cyChar;
         break;
      case SB_THUMBPOSITION:
         position = LOWORD(lParam);
         break;
      }
      position = max(0, min(position, numLines));

      if ( position != GetScrollPos(scroll, SB_CTL) )
      {
         SetScrollPos(scroll, SB_CTL, position, TRUE);
         InvalidateRect(hwnd, NULL, TRUE);
      }
      return 0;

   case WM_PAINT:
      hdc = BeginPaint(hwnd, &ps);

      //pText = (char*)LockResource(hResource);

      GetClientRect(hwnd, &rect);
      rect.left += cxChar;
      rect.top += cyChar * (1 - position);
      DrawTextA(hdc, rsc, -1, &rect, DT_EXTERNALLEADING);

      EndPaint(hwnd, &ps);
      return 0;

   case WM_DESTROY:
      FreeResource(resource);
      free(rsc);
      PostQuitMessage(0);
      return 0;
   }
   return DefWindowProc(hwnd, message, wParam, lParam);
}
