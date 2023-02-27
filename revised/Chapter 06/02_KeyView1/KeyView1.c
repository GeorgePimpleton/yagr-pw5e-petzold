/*--------------------------------------------------------
   KEYVIEW1.C -- Displays Keyboard and Character Messages
             (c) Charles Petzold, 1998
  --------------------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(_In_     HINSTANCE hInstance,
                    _In_opt_ HINSTANCE hPrevInstance,
                    _In_     PWSTR     pCmdLine,
                    _In_     int       nShowCmd)
{
   UNREFERENCED_PARAMETER(hPrevInstance);
   UNREFERENCED_PARAMETER(pCmdLine);

   static PCWSTR szAppName = L"KeyView1";
   HWND          hwnd;
   MSG           msg;
   WNDCLASSW     wc;

   wc.style         = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = hInstance;
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

   hwnd = CreateWindowW(szAppName, L"Keyboard Message Viewer #1",
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        NULL, NULL, hInstance, NULL);

   ShowWindow(hwnd, nShowCmd);
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
   static int    cxClientMax;
   static int    cyClientMax;
   static int    cxClient;
   static int    cyClient;
   static int    cxChar;
   static int    cyChar;
   static int    cLinesMax;
   static int    cLines;
   static PMSG   pmsg;
   static RECT   rectScroll;
   static PCWSTR top  = L"Message        Key       Char     Repeat Scan Ext ALT Prev Tran";
   static PCWSTR und  = L"_______        ___       ____     ______ ____ ___ ___ ____ ____";

   static PCWSTR format[ 2 ] = { L"%-13s %3d %-15s%c%6u %4d %3s %3s %4s %4s",
                                 L"%-13s            0x%04X%1s%c %6u %4d %3s %3s %4s %4s" };

   static PCWSTR yes  = L"Yes";
   static PCWSTR no   = L"No";
   static PCWSTR down = L"Down";
   static PCWSTR up   = L"Up";

   static PCWSTR msgList[] = { L"WM_KEYDOWN",    L"WM_KEYUP",
                               L"WM_CHAR",       L"WM_DEADCHAR",
                               L"WM_SYSKEYDOWN", L"WM_SYSKEYUP",
                               L"WM_SYSCHAR",    L"WM_SYSDEADCHAR" };
   HDC           hdc;
   int           i;
   int           iType;
   PAINTSTRUCT   ps;
   WCHAR         szBuffer[ 128 ];
   WCHAR         szKeyName[ 32 ];
   TEXTMETRICW   tm;

   switch ( message )
   {
   case WM_CREATE:
   case WM_DISPLAYCHANGE:
      // get maximum size of client area
      cxClientMax = GetSystemMetrics(SM_CXMAXIMIZED);
      cyClientMax = GetSystemMetrics(SM_CYMAXIMIZED);

      // get character size for fixed-pitch font
      hdc = GetDC(hwnd);

      SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
      GetTextMetrics(hdc, &tm);
      cxChar = tm.tmAveCharWidth;
      cyChar = tm.tmHeight;

      ReleaseDC(hwnd, hdc);

      // allocate memory for display lines
      if ( pmsg )
         free(pmsg);

      cLinesMax = cyClientMax / cyChar;
      pmsg      = (PMSG) malloc(cLinesMax * sizeof(MSG));
      cLines    = 0;

      // fall through

   case WM_SIZE:
      if ( message == WM_SIZE )
      {
         cxClient = GET_X_LPARAM(lParam);
         cyClient = GET_Y_LPARAM(lParam);
      }

      // calculate scrolling rectangle
      rectScroll.left   = 0;
      rectScroll.right  = cxClient;
      rectScroll.top    = cyChar;
      rectScroll.bottom = cyChar * (cyClient / cyChar);

      InvalidateRect(hwnd, NULL, TRUE);
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
      for ( i = cLinesMax - 1; i > 0; i-- )
      {
         pmsg[ i ] = pmsg[ i - 1 ];
      }
      // Store new message

      pmsg[ 0 ].hwnd = hwnd;
      pmsg[ 0 ].message = message;
      pmsg[ 0 ].wParam = wParam;
      pmsg[ 0 ].lParam = lParam;

      cLines = min(cLines + 1, cLinesMax);

      // scroll up the display
      ScrollWindow(hwnd, 0, -cyChar, &rectScroll, &rectScroll);

      break;   // ie, call DefWindowProc so Sys messages work

   case WM_PAINT:
      hdc = BeginPaint(hwnd, &ps);

      SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
      SetBkMode(hdc, TRANSPARENT);
      TextOutW(hdc, 1, 0, top, lstrlenW(top));
      TextOutW(hdc, 1, 0, und, lstrlenW(und));

      for ( i = 0; i < min(cLines, cyClient / cyChar - 1); i++ )
      {
         iType = pmsg[ i ].message == WM_CHAR ||
                 pmsg[ i ].message == WM_SYSCHAR ||
                 pmsg[ i ].message == WM_DEADCHAR ||
                 pmsg[ i ].message == WM_SYSDEADCHAR;

         GetKeyNameTextW((LONG) pmsg[ i ].lParam, szKeyName, _countof(szKeyName));

         TextOutW(hdc, 1, (cyClient / cyChar - 1 - i) * cyChar, szBuffer,
                  wsprintfW(szBuffer, format[ iType ],
                            msgList[ pmsg[ i ].message - WM_KEYFIRST ],
                            pmsg[ i ].wParam,
                            (PTSTR) (iType ? L" " : szKeyName),
                            (TCHAR) (iType ? pmsg[ i ].wParam : ' '),
                            LOWORD(pmsg[ i ].lParam),
                            HIWORD(pmsg[ i ].lParam) & 0xFF,
                            0x01000000 & pmsg[ i ].lParam ? yes : no,
                            0x20000000 & pmsg[ i ].lParam ? yes : no,
                            0x40000000 & pmsg[ i ].lParam ? down : up,
                            0x80000000 & pmsg[ i ].lParam ? up : down));
      }
      EndPaint(hwnd, &ps);
      return 0;

   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(hwnd, message, wParam, lParam);
}