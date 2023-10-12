/*-----------------------------------------
   DIGCLOCK.c -- Digital Clock
                 (c) Charles Petzold, 1998
  -----------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>

#define ID_TIMER    1

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(_In_     HINSTANCE inst,
                    _In_opt_ HINSTANCE prevInst,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInst);
   UNREFERENCED_PARAMETER(cmdLine);

   static PCWSTR  appName = L"DigClock";
   HWND           wnd;
   MSG            msg;
   WNDCLASSW      wc;

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
      MessageBoxW(NULL, L"Program requires Windows NT!", appName, MB_ICONERROR);
      return 0;
   }

   wnd = CreateWindowW(appName, L"Digital Clock",
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

void DisplayDigit(HDC dc, int number)
{
   static BOOL  sevenSegment[ 10 ][ 7 ] = { {1, 1, 1, 0, 1, 1, 1},   // 0
                                            {0, 0, 1, 0, 0, 1, 0},   // 1
                                            {1, 0, 1, 1, 1, 0, 1},   // 2
                                            {1, 0, 1, 1, 0, 1, 1},   // 3
                                            {0, 1, 1, 1, 0, 1, 0},   // 4
                                            {1, 1, 0, 1, 0, 1, 1},   // 5
                                            {1, 1, 0, 1, 1, 1, 1},   // 6
                                            {1, 0, 1, 0, 0, 1, 0},   // 7
                                            {1, 1, 1, 1, 1, 1, 1},   // 8
                                            {1, 1, 1, 1, 0, 1, 1} }; // 9
   static POINT segment[ 7 ][ 6 ] = { {{   7,  6},  {11,  2},  {31,  2},  {35,  6},  {31, 10},  {11, 10}},
                                      {{   6,  7},  {10, 11},  {10, 31},   {6, 35},   {2, 31},   {2, 11}},
                                      {{  36,  7},  {40, 11},  {40, 31},  {36, 35},  {32, 31},  {32, 11}},
                                      {{   7, 36},  {11, 32},  {31, 32},  {35, 36},  {31, 40},  {11, 40}},
                                      {{   6, 37},  {10, 41},  {10, 61},   {6, 65},   {2, 61},   {2, 41}},
                                      {{  36, 37},  {40, 41},  {40, 61},  {36, 65},  {32, 61},  {32, 41}},
                                      {{   7, 66},  {11, 62},  {31, 62},  {35, 66},  {31, 70},  {11, 70 } } };
   int          seg;

   for ( seg = 0; seg < 7; seg++ )
   {
      if ( sevenSegment[ number ][ seg ] )
      {
         Polygon(dc, segment[ seg ], 6);
      }
   }
}

void DisplayTwoDigits(HDC dc, int number, BOOL suppress)
{
   if ( !suppress || (number / 10 != 0) )
   {
      DisplayDigit(dc, number / 10);
   }

   OffsetWindowOrgEx(dc, -42, 0, NULL);
   DisplayDigit(dc, number % 10);
   OffsetWindowOrgEx(dc, -42, 0, NULL);
}

void DisplayColon(HDC dc)
{
   POINT colon[ 2 ][ 4 ] = { { {2,  21},  {6,  17},  {10, 21},  {6, 25},},
                             { {2,  51},  {6,  47},  {10, 51},  {6, 55 }} };

   Polygon(dc, colon[ 0 ], 4);
   Polygon(dc, colon[ 1 ], 4);

   OffsetWindowOrgEx(dc, -12, 0, NULL);
}

void DisplayTime(HDC dc, BOOL hour24, BOOL suppress)
{
   SYSTEMTIME st;

   GetLocalTime(&st);

   if ( hour24 )
   {
      DisplayTwoDigits(dc, st.wHour, suppress);
   }
   else
   {
      DisplayTwoDigits(dc, (st.wHour %= 12) ? st.wHour : 12, suppress);
   }

   DisplayColon(dc);
   DisplayTwoDigits(dc, st.wMinute, FALSE);
   DisplayColon(dc);
   DisplayTwoDigits(dc, st.wSecond, FALSE);
}

LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static BOOL   hour24;
   static BOOL   suppress;
   static HBRUSH brushRed;
   static int    xClient;
   static int    yClient;
   HDC           dc;
   PAINTSTRUCT   ps;
   WCHAR         buffer[ 2 ];

   switch ( msg )
   {
   case WM_CREATE:
      brushRed = CreateSolidBrush(RGB(255, 0, 0));
      SetTimer(wnd, ID_TIMER, 1000, NULL);

      // fall through

   case WM_SETTINGCHANGE:
      GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_ITIME, buffer, 2);
      hour24 = (buffer[ 0 ] == '1');

      GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_ITLZERO, buffer, 2);
      suppress = (buffer[ 0 ] == '0');

      InvalidateRect(wnd, NULL, TRUE);
      return 0;

   case WM_SIZE:
      xClient = GET_X_LPARAM(lParam);
      yClient = GET_Y_LPARAM(lParam);
      return 0;

   case WM_TIMER:
      InvalidateRect(wnd, NULL, TRUE);
      return 0;

   case WM_PAINT:
      dc = BeginPaint(wnd, &ps);

      SetMapMode(dc, MM_ISOTROPIC);
      SetWindowExtEx(dc, 276, 72, NULL);
      SetViewportExtEx(dc, xClient, yClient, NULL);

      SetWindowOrgEx(dc, 138, 36, NULL);
      SetViewportOrgEx(dc, xClient / 2, yClient / 2, NULL);

      SelectObject(dc, GetStockObject(NULL_PEN));
      SelectObject(dc, brushRed);

      DisplayTime(dc, hour24, suppress);

      EndPaint(wnd, &ps);
      return 0;

   case WM_DESTROY:
      KillTimer(wnd, ID_TIMER);
      DeleteObject(brushRed);
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(wnd, msg, wParam, lParam);
}