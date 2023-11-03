/*-----------------------------------------
   CLIPVIEW.C -- Simple Clipboard Viewer
                 (c) Charles Petzold, 1998
  -----------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(_In_     HINSTANCE inst,
                    _In_opt_ HINSTANCE prevInst,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(cmdLine);

   PCWSTR    appName = L"ClipView";
   HWND      wnd;
   MSG       msg;
   WNDCLASSW wc      = { 0 };

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

   if ( !RegisterClassW(&wc) )
   {
      MessageBoxW(NULL, L"This program requires Windows NT!", appName, MB_ICONERROR);
      return 0;
   }

   wnd = CreateWindowW(appName, L"Simple Clipboard Viewer (Text Only)",
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
   static HWND wndNextViewer;
   HGLOBAL     global;
   HDC         dc;
   PWSTR       strGlobal;
   PAINTSTRUCT ps;
   RECT        rect;

   switch ( msg )
   {
   case WM_CREATE:
      wndNextViewer = SetClipboardViewer(wnd);
      return 0;

   case WM_CHANGECBCHAIN:
      if ( (HWND) wParam == wndNextViewer )
      {
         wndNextViewer = (HWND) lParam;
      }
      else if ( wndNextViewer )
      {
         SendMessageW(wndNextViewer, msg, wParam, lParam);
      }

      return 0;

   case WM_DRAWCLIPBOARD:
      if ( wndNextViewer )
      {
         SendMessageW(wndNextViewer, msg, wParam, lParam);
      }

      InvalidateRect(wnd, NULL, TRUE);
      return 0;

   case WM_PAINT:
      dc = BeginPaint(wnd, &ps);
      GetClientRect(wnd, &rect);
      OpenClipboard(wnd);

      global = GetClipboardData(CF_UNICODETEXT);

      if ( global != NULL )
      {
         strGlobal = (PWSTR) GlobalLock(global);
         DrawTextW(dc, strGlobal, -1, &rect, DT_EXPANDTABS);
         GlobalUnlock(global);
      }

      CloseClipboard( );
      EndPaint(wnd, &ps);
      return 0;

   case WM_DESTROY:
      ChangeClipboardChain(wnd, wndNextViewer);
      PostQuitMessage(0);
      return 0;
   }
   return DefWindowProcW(wnd, msg, wParam, lParam);
}