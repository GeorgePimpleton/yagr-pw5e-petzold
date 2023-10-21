/*-------------------------------------------------
   CHECKER2.C -- Mouse Hit-Test Demo Program No. 2
             (c) Charles Petzold, 1998
  -------------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#define DIVISIONS 5

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(_In_     HINSTANCE inst,
                    _In_opt_ HINSTANCE prevInst,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInst);
   UNREFERENCED_PARAMETER(cmdLine);

   static PCWSTR appName = L"Checker2";
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

   wnd = CreateWindowW(appName, L"Checker2 Mouse Hit-Test Demo",
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

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   static BOOL state[ DIVISIONS ][ DIVISIONS ];
   static int  xBlock;
   static int  yBlock;
   HDC         dc;
   int         x;
   int         y;
   PAINTSTRUCT ps;
   POINT       point;
   RECT        rect = { 0 };

   switch ( message )
   {
   case WM_SIZE:
      xBlock = LOWORD(lParam) / DIVISIONS;
      yBlock = HIWORD(lParam) / DIVISIONS;
      return 0;

   case WM_SETFOCUS:
      ShowCursor(TRUE);
      return 0;

   case WM_KILLFOCUS:
      ShowCursor(FALSE);
      return 0;

   case WM_KEYDOWN:
      GetCursorPos(&point);
      ScreenToClient(hwnd, &point);

      x = max(0, min(DIVISIONS - 1, point.x / xBlock));
      y = max(0, min(DIVISIONS - 1, point.y / yBlock));

      switch ( wParam )
      {
      case VK_UP:
         y--;
         break;

      case VK_DOWN:
         y++;
         break;

      case VK_LEFT:
         x--;
         break;

      case VK_RIGHT:
         x++;
         break;

      case VK_HOME:
         x = y = 0;
         break;

      case VK_END:
         x = y = DIVISIONS - 1;
         break;

      case VK_RETURN:
      case VK_SPACE:
         SendMessageW(hwnd, WM_LBUTTONDOWN, MK_LBUTTON,
                      MAKELONG(x * xBlock, y * yBlock));
         break;
      }
      x = (x + DIVISIONS) % DIVISIONS;
      y = (y + DIVISIONS) % DIVISIONS;

      point.x = x * xBlock + xBlock / 2;
      point.y = y * yBlock + yBlock / 2;

      ClientToScreen(hwnd, &point);
      SetCursorPos(point.x, point.y);
      return 0;

   case WM_LBUTTONDOWN:
      x = LOWORD(lParam) / xBlock;
      y = HIWORD(lParam) / yBlock;

      if ( x < DIVISIONS && y < DIVISIONS )
      {
         state[ x ][ y ] ^= 1;

         rect.left   = x * xBlock;
         rect.top    = y * yBlock;
         rect.right  = (x + 1) * xBlock;
         rect.bottom = (y + 1) * yBlock;

         InvalidateRect(hwnd, &rect, FALSE);
      }
      else
         MessageBeep(0);
      return 0;

   case WM_PAINT:
      dc = BeginPaint(hwnd, &ps);

      for ( x = 0; x < DIVISIONS; x++ )
      {
         for ( y = 0; y < DIVISIONS; y++ )
         {
            Rectangle(dc, x * xBlock, y * yBlock,
                      (x + 1) * xBlock, (y + 1) * yBlock);

            if ( state[ x ][ y ] )
            {
               MoveToEx(dc, x * xBlock, y * yBlock, NULL);
               LineTo(dc, (x + 1) * xBlock, (y + 1) * yBlock);
               MoveToEx(dc, x * xBlock, (y + 1) * yBlock, NULL);
               LineTo(dc, (x + 1) * xBlock, y * yBlock);
            }
         }
      }

      EndPaint(hwnd, &ps);
      return 0;

   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(hwnd, message, wParam, lParam);
}