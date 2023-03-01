/*-----------------------------------------
   DIGCLOCK.c -- Digital Clock
             (c) Charles Petzold, 1998
  -----------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>

#define ID_TIMER    1

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(_In_     HINSTANCE instance,
                    _In_opt_ HINSTANCE prevInstance,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInstance);
   UNREFERENCED_PARAMETER(cmdLine);

   static PCWSTR  appName = L"DigClock";
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

   hwnd = CreateWindowW(appName, L"Digital Clock",
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

void DisplayDigit(HDC hdc, int iNumber)
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
      if ( sevenSegment[ iNumber ][ seg ] )
         Polygon(hdc, segment[ seg ], 6);
}

void DisplayTwoDigits(HDC hdc, int iNumber, BOOL fSuppress)
{
   if ( !fSuppress || (iNumber / 10 != 0) )
      DisplayDigit(hdc, iNumber / 10);

   OffsetWindowOrgEx(hdc, -42, 0, NULL);
   DisplayDigit(hdc, iNumber % 10);
   OffsetWindowOrgEx(hdc, -42, 0, NULL);
}

void DisplayColon(HDC hdc)
{
   POINT colon[ 2 ][ 4 ] = { { {2,  21},  {6,  17},  {10, 21},  {6, 25},},
                             { {2,  51},  {6,  47},  {10, 51},  {6, 55 }} };

   Polygon(hdc, colon[ 0 ], 4);
   Polygon(hdc, colon[ 1 ], 4);

   OffsetWindowOrgEx(hdc, -12, 0, NULL);
}

void DisplayTime(HDC hdc, BOOL f24Hour, BOOL fSuppress)
{
   SYSTEMTIME st;

   GetLocalTime(&st);

   if ( f24Hour )
      DisplayTwoDigits(hdc, st.wHour, fSuppress);
   else
      DisplayTwoDigits(hdc, (st.wHour %= 12) ? st.wHour : 12, fSuppress);

   DisplayColon(hdc);
   DisplayTwoDigits(hdc, st.wMinute, FALSE);
   DisplayColon(hdc);
   DisplayTwoDigits(hdc, st.wSecond, FALSE);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   static BOOL   f24Hour;
   static BOOL   fSuppress;
   static HBRUSH brushRed;
   static int    cxClient;
   static int    cyClient;
   HDC           hdc;
   PAINTSTRUCT   ps;
   WCHAR         buffer[ 2 ];

   switch ( message )
   {
   case WM_CREATE:
      brushRed = CreateSolidBrush(RGB(255, 0, 0));
      SetTimer(hwnd, ID_TIMER, 1000, NULL);

      // fall through

   case WM_SETTINGCHANGE:
      GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_ITIME, buffer, 2);
      f24Hour = (buffer[ 0 ] == '1');

      GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_ITLZERO, buffer, 2);
      fSuppress = (buffer[ 0 ] == '0');

      InvalidateRect(hwnd, NULL, TRUE);
      return 0;

   case WM_SIZE:
      cxClient = GET_X_LPARAM(lParam);
      cyClient = GET_Y_LPARAM(lParam);
      return 0;

   case WM_TIMER:
      InvalidateRect(hwnd, NULL, TRUE);
      return 0;

   case WM_PAINT:
      hdc = BeginPaint(hwnd, &ps);

      SetMapMode(hdc, MM_ISOTROPIC);
      SetWindowExtEx(hdc, 276, 72, NULL);
      SetViewportExtEx(hdc, cxClient, cyClient, NULL);

      SetWindowOrgEx(hdc, 138, 36, NULL);
      SetViewportOrgEx(hdc, cxClient / 2, cyClient / 2, NULL);

      SelectObject(hdc, GetStockObject(NULL_PEN));
      SelectObject(hdc, brushRed);

      DisplayTime(hdc, f24Hour, fSuppress);

      EndPaint(hwnd, &ps);
      return 0;

   case WM_DESTROY:
      KillTimer(hwnd, ID_TIMER);
      DeleteObject(brushRed);
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(hwnd, message, wParam, lParam);
}