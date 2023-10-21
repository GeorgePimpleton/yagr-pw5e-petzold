/*-------------------------------------------------
   CHECKER3.C -- Mouse Hit-Test Demo Program No. 3
                 (c) Charles Petzold, 1998
  -------------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#define DIVISIONS 5

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChildWndProc(HWND, UINT, WPARAM, LPARAM);

PCWSTR childClass = L"Checker3_Child";

int WINAPI wWinMain(_In_     HINSTANCE inst,
                    _In_opt_ HINSTANCE prevInst,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInst);
   UNREFERENCED_PARAMETER(cmdLine);

   static PCWSTR appName = L"Checker3";
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

   /* register the child window class */
   wc.lpfnWndProc   = ChildWndProc;
   wc.cbWndExtra    = sizeof(long);
   wc.hIcon         = NULL;
   wc.lpszClassName = childClass;

   RegisterClassW(&wc);

   wnd = CreateWindowW(appName, L"Checker3 Mouse Hit-Test Demo",
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
   static HWND wndChild[ DIVISIONS ][ DIVISIONS ];
   int         xBlock;
   int         yBlock;
   int         x;
   int         y;

   switch ( msg )
   {
   case WM_CREATE:
      for ( x = 0; x < DIVISIONS; x++ )
      {
         for ( y = 0; y < DIVISIONS; y++ )
         {
            wndChild[ x ][ y ] = CreateWindowW(childClass, NULL,
                                               WS_CHILDWINDOW | WS_VISIBLE,
                                               0, 0, 0, 0,
                                               wnd, (HMENU) (y << 8 | x),
                                               (HINSTANCE) GetWindowLongPtrW(wnd, GWLP_HINSTANCE),
                                               NULL);
         }
      }
      return 0;

   case WM_SIZE:
      xBlock = LOWORD(lParam) / DIVISIONS;
      yBlock = HIWORD(lParam) / DIVISIONS;

      for ( x = 0; x < DIVISIONS; x++ )
      {
         for ( y = 0; y < DIVISIONS; y++ )
         {
            MoveWindow(wndChild[ x ][ y ],
                       x * xBlock, y * yBlock,
                       xBlock, yBlock, TRUE);
         }
      }
      return 0;

   case WM_LBUTTONDOWN:
      MessageBeep(0);
      return 0;

   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(wnd, msg, wParam, lParam);
}

LRESULT CALLBACK ChildWndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   HDC         dc;
   PAINTSTRUCT ps;
   RECT        rect;

   switch ( msg )
   {
   case WM_CREATE:
      SetWindowLongPtrW(wnd, GWLP_USERDATA, 0);       // on/off flag
      return 0;

   case WM_LBUTTONDOWN:
      SetWindowLongPtrW(wnd, GWLP_USERDATA, 1 ^ GetWindowLongPtrW(wnd, GWLP_USERDATA));
      InvalidateRect(wnd, NULL, FALSE);
      return 0;

   case WM_PAINT:
      dc = BeginPaint(wnd, &ps);

      GetClientRect(wnd, &rect);
      Rectangle(dc, 0, 0, rect.right, rect.bottom);

      if ( GetWindowLongPtrW(wnd, GWLP_USERDATA) )
      {
         MoveToEx(dc, 0, 0, NULL);
         LineTo(dc, rect.right, rect.bottom);
         MoveToEx(dc, 0, rect.bottom, NULL);
         LineTo(dc, rect.right, 0);
      }

      EndPaint(wnd, &ps);
      return 0;
   }

   return DefWindowProcW(wnd, msg, wParam, lParam);
}