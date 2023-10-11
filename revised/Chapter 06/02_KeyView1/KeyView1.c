/*--------------------------------------------------------
   KEYVIEW1.C -- Displays Keyboard and Character Messages
                 (c) Charles Petzold, 1998
  --------------------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(_In_     HINSTANCE inst,
                    _In_opt_ HINSTANCE prevInst,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInst);
   UNREFERENCED_PARAMETER(cmdLine);

   static PCWSTR szAppName = L"KeyView1";
   HWND          wnd;
   MSG           msg;
   WNDCLASSW     wc;

   wc.style         = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = inst;
   wc.hIcon         = (HICON)   LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wc.hbrBackground = (HBRUSH)  (COLOR_WINDOW + 1);
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = szAppName;

   if ( !RegisterClassW(&wc) )
   {
      MessageBoxW(NULL, L"This program requires Windows NT!",
                  szAppName, MB_ICONERROR);
      return 0;
   }

   wnd = CreateWindowW(szAppName, L"Keyboard Message Viewer #1",
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
   static int    xClientMax;
   static int    yClientMax;
   static int    xClient;
   static int    yClient;
   static int    xChar;
   static int    yChar;
   static int    linesMax;
   static int    lines;
   static PMSG   pmsg;
   static RECT   rectScroll;

   static PCWSTR top = L"Message        Key       Char     Repeat Scan Ext ALT Prev Tran";
   static PCWSTR und = L"_______        ___       ____     ______ ____ ___ ___ ____ ____";

   static PCWSTR format[ 2 ] = { L"%-13s %3d %-15s%c%6u %4d %3s %3s %4s %4s",
                                 L"%-13s            0x%04X%1s%c %6u %4d %3s %3s %4s %4s" };

   static PCWSTR yes  = L"Yes";
   static PCWSTR no   = L"No";
   static PCWSTR down = L"Down";
   static PCWSTR up   = L"Up";

   static PCWSTR msgList[ ] = { L"WM_KEYDOWN",    L"WM_KEYUP",
                               L"WM_CHAR",       L"WM_DEADCHAR",
                               L"WM_SYSKEYDOWN", L"WM_SYSKEYUP",
                               L"WM_SYSCHAR",    L"WM_SYSDEADCHAR" };
   HDC           dc;
   int           i;
   int           type;
   PAINTSTRUCT   ps;
   WCHAR         buffer[ 128 ];
   WCHAR         keyName[ 32 ];
   TEXTMETRICW   tm;

   switch ( msg )
   {
   case WM_CREATE:
   case WM_DISPLAYCHANGE:
      // get maximum size of client area
      xClientMax = GetSystemMetrics(SM_CXMAXIMIZED);
      yClientMax = GetSystemMetrics(SM_CYMAXIMIZED);

      // get character size for fixed-pitch font
      dc = GetDC(wnd);

      SelectObject(dc, GetStockObject(SYSTEM_FIXED_FONT));
      GetTextMetricsW(dc, &tm);
      xChar = tm.tmAveCharWidth;
      yChar = tm.tmHeight;

      ReleaseDC(wnd, dc);

      // allocate memory for display lines
      if ( pmsg )
      {
         free(pmsg);
      }

      linesMax = yClientMax / yChar;
      pmsg     = (PMSG) malloc(linesMax * sizeof(MSG));
      lines    = 0;

      // fall through

   case WM_SIZE:
      if ( msg == WM_SIZE )
      {
         xClient = GET_X_LPARAM(lParam);
         yClient = GET_Y_LPARAM(lParam);
      }

      // calculate scrolling rectangle
      rectScroll.left   = 0;
      rectScroll.right  = xClient;
      rectScroll.top    = yChar;
      rectScroll.bottom = yChar * (yClient / yChar);

      InvalidateRect(wnd, NULL, TRUE);
      return 0;

   case WM_KEYDOWN:
   case WM_KEYUP:
   case WM_CHAR:
   case WM_DEADCHAR:
   case WM_SYSKEYDOWN:
   case WM_SYSKEYUP:
   case WM_SYSCHAR:
   case WM_SYSDEADCHAR:
      // rearrange storage array
      for ( i = linesMax - 1; i > 0; i-- )
      {
         pmsg[ i ] = pmsg[ i - 1 ];
      }

      // store new message
      pmsg[ 0 ].hwnd    = wnd;
      pmsg[ 0 ].message = msg;
      pmsg[ 0 ].wParam  = wParam;
      pmsg[ 0 ].lParam  = lParam;

      lines = min(lines + 1, linesMax);

      // scroll up the display
      ScrollWindow(wnd, 0, -yChar, &rectScroll, &rectScroll);

      break;   // ie, call DefWindowProc so Sys messages work

   case WM_PAINT:
      dc = BeginPaint(wnd, &ps);

      SelectObject(dc, GetStockObject(SYSTEM_FIXED_FONT));
      SetBkMode(dc, TRANSPARENT);
      TextOutW(dc, 1, 0, top, lstrlenW(top));
      TextOutW(dc, 1, 0, und, lstrlenW(und));

      for ( i = 0; i < min(lines, yClient / yChar - 1); i++ )
      {
         type = pmsg[ i ].message == WM_CHAR ||
                pmsg[ i ].message == WM_SYSCHAR ||
                pmsg[ i ].message == WM_DEADCHAR ||
                pmsg[ i ].message == WM_SYSDEADCHAR;

         GetKeyNameTextW((LONG) pmsg[ i ].lParam, keyName, _countof(keyName));

         TextOutW(dc, 1, (yClient / yChar - 1 - i) * yChar, buffer,
                  wsprintfW(buffer, format[ type ],
                            msgList[ pmsg[ i ].message - WM_KEYFIRST ],
                            pmsg[ i ].wParam,
                            (PWSTR) (type ? L" " : keyName),
                            (WCHAR) (type ? pmsg[ i ].wParam : L' '),
                            LOWORD(pmsg[ i ].lParam),
                            HIWORD(pmsg[ i ].lParam) & 0xFF,
                            0x01000000 & pmsg[ i ].lParam ? yes : no,
                            0x20000000 & pmsg[ i ].lParam ? yes : no,
                            0x40000000 & pmsg[ i ].lParam ? down : up,
                            0x80000000 & pmsg[ i ].lParam ? up : down));
      }
      EndPaint(wnd, &ps);
      return 0;

   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(wnd, msg, wParam, lParam);
}