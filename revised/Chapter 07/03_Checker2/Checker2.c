/*-------------------------------------------------
   CHECKER2.C -- Mouse Hit-Test Demo Program No. 2
             (c) Charles Petzold, 1998
  -------------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#define DIVISIONS 5

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(_In_     HINSTANCE instance,
                    _In_opt_ HINSTANCE prevInstance,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInstance);
   UNREFERENCED_PARAMETER(cmdLine);

   static PCWSTR  appName = L"Checker2";
   HWND           hwnd;
   MSG            msg;
   WNDCLASSW      wc;

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
      MessageBoxW(NULL, L"Program requires Windows NT!",
                  appName, MB_ICONERROR);
      return 0;
   }

   hwnd = CreateWindowW(appName, L"Checker2 Mouse Hit-Test Demo",
                        WS_OVERLAPPEDWINDOW,
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
   static BOOL state[ DIVISIONS ][ DIVISIONS ];
   static int  cxBlock;
   static int  cyBlock;
   HDC         hdc;
   int         x;
   int         y;
   PAINTSTRUCT ps;
   POINT       point;
   RECT        rect;

   switch ( message )
   {
   case WM_SIZE:
      cxBlock = LOWORD(lParam) / DIVISIONS;
      cyBlock = HIWORD(lParam) / DIVISIONS;
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

      x = max(0, min(DIVISIONS - 1, point.x / cxBlock));
      y = max(0, min(DIVISIONS - 1, point.y / cyBlock));

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
                      MAKELONG(x * cxBlock, y * cyBlock));
         break;
      }
      x = (x + DIVISIONS) % DIVISIONS;
      y = (y + DIVISIONS) % DIVISIONS;

      point.x = x * cxBlock + cxBlock / 2;
      point.y = y * cyBlock + cyBlock / 2;

      ClientToScreen(hwnd, &point);
      SetCursorPos(point.x, point.y);
      return 0;

   case WM_LBUTTONDOWN:
      x = LOWORD(lParam) / cxBlock;
      y = HIWORD(lParam) / cyBlock;

      if ( x < DIVISIONS && y < DIVISIONS )
      {
         state[ x ][ y ] ^= 1;

         rect.left = x * cxBlock;
         rect.top = y * cyBlock;
         rect.right = (x + 1) * cxBlock;
         rect.bottom = (y + 1) * cyBlock;

         InvalidateRect(hwnd, &rect, FALSE);
      }
      else
         MessageBeep(0);
      return 0;

   case WM_PAINT:
      hdc = BeginPaint(hwnd, &ps);

      for ( x = 0; x < DIVISIONS; x++ )
         for ( y = 0; y < DIVISIONS; y++ )
         {
            Rectangle(hdc, x * cxBlock, y * cyBlock,
                      (x + 1) * cxBlock, (y + 1) * cyBlock);

            if ( state[ x ][ y ] )
            {
               MoveToEx(hdc, x * cxBlock, y * cyBlock, NULL);
               LineTo(hdc, (x + 1) * cxBlock, (y + 1) * cyBlock);
               MoveToEx(hdc, x * cxBlock, (y + 1) * cyBlock, NULL);
               LineTo(hdc, (x + 1) * cxBlock, y * cyBlock);
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