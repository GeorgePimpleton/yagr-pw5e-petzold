/*--------------------------------------
   CLOCK.C -- Analog Clock Program
           (c) Charles Petzold, 1998
  --------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>

#define _USE_MATH_DEFINES
#include <math.h>

#define ID_TIMER    1
#define TWOPI       (2 * M_PI)

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(_In_     HINSTANCE instance,
                    _In_opt_ HINSTANCE prevInstance,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInstance);
   UNREFERENCED_PARAMETER(cmdLine);

   static PCWSTR  appName = L"Clock";
   HWND           hwnd;
   MSG            msg;
   WNDCLASSW      wc;

   wc.style         = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = instance;
   wc.hIcon         = NULL;
   wc.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wc.hbrBackground = (HBRUSH)  GetStockObject(WHITE_BRUSH);
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = appName;

   if ( !RegisterClassW(&wc) )
   {
      MessageBoxW(NULL, L"Program requires Windows NT!",
                  appName, MB_ICONERROR);
      return 0;
   }

   hwnd = CreateWindowW(appName, L"Analog Clock",
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

void SetIsotropic(HDC hdc, int cxClient, int cyClient)
{
   SetMapMode(hdc, MM_ISOTROPIC);
   SetWindowExtEx(hdc, 1000, 1000, NULL);
   SetViewportExtEx(hdc, cxClient / 2, -cyClient / 2, NULL);
   SetViewportOrgEx(hdc, cxClient / 2, cyClient / 2, NULL);
}

void RotatePoint(POINT pt[], int iNum, int iAngle)
{
   int   i;
   POINT temp;

   for ( i = 0; i < iNum; i++ )
   {
      temp.x = (int) (pt[ i ].x * cos(TWOPI * iAngle / 360) +
                      pt[ i ].y * sin(TWOPI * iAngle / 360));

      temp.y = (int) (pt[ i ].y * cos(TWOPI * iAngle / 360) -
                      pt[ i ].x * sin(TWOPI * iAngle / 360));

      pt[ i ] = temp;
   }
}

void DrawClock(HDC hdc)
{
   int   angle;
   POINT pt[ 3 ];

   for ( angle = 0; angle < 360; angle += 6 )
   {
      pt[ 0 ].x = 0;
      pt[ 0 ].y = 900;

      RotatePoint(pt, 1, angle);

      pt[ 2 ].x = pt[ 2 ].y = angle % 5 ? 33 : 100;

      pt[ 0 ].x -= pt[ 2 ].x / 2;
      pt[ 0 ].y -= pt[ 2 ].y / 2;

      pt[ 1 ].x = pt[ 0 ].x + pt[ 2 ].x;
      pt[ 1 ].y = pt[ 0 ].y + pt[ 2 ].y;

      SelectObject(hdc, GetStockObject(BLACK_BRUSH));

      Ellipse(hdc, pt[ 0 ].x, pt[ 0 ].y, pt[ 1 ].x, pt[ 1 ].y);
   }
}

void DrawHands(HDC hdc, SYSTEMTIME* pst, BOOL fChange)
{
   static POINT pt[ 3 ][ 5 ] = { {{0, -150}, {100, 0}, {0, 600}, {-100, 0}, {0, -150}},
                                 {{0, -200}, { 50, 0}, {0, 800}, { -50, 0}, {0, -200}},
                                 {{0,    0}, {  0, 0}, {0,   0}, {   0, 0}, {0,  800} } };
   int          i;
   int          angle[ 3 ];
   POINT        temp[ 3 ][ 5 ];

   angle[ 0 ] = (pst->wHour * 30) % 360 + pst->wMinute / 2;
   angle[ 1 ] = pst->wMinute * 6;
   angle[ 2 ] = pst->wSecond * 6;

   memcpy(temp, pt, sizeof(pt));

   for ( i = fChange ? 0 : 2; i < 3; i++ )
   {
      RotatePoint(temp[ i ], 5, angle[ i ]);

      Polyline(hdc, temp[ i ], 5);
   }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   static int        cxClient, cyClient;
   static SYSTEMTIME previousTime;
   BOOL              change;
   HDC               hdc;
   PAINTSTRUCT       ps;
   SYSTEMTIME        st;

   switch ( message )
   {
   case WM_CREATE:
      SetTimer(hwnd, ID_TIMER, 1000, NULL);
      GetLocalTime(&st);
      previousTime = st;
      return 0;

   case WM_SIZE:
      cxClient = GET_X_LPARAM(lParam);
      cyClient = GET_Y_LPARAM(lParam);
      return 0;

   case WM_TIMER:
      GetLocalTime(&st);

      change = st.wHour != previousTime.wHour ||
         st.wMinute != previousTime.wMinute;

      hdc = GetDC(hwnd);

      SetIsotropic(hdc, cxClient, cyClient);

      SelectObject(hdc, GetStockObject(WHITE_PEN));
      DrawHands(hdc, &previousTime, change);

      SelectObject(hdc, GetStockObject(BLACK_PEN));
      DrawHands(hdc, &st, TRUE);

      ReleaseDC(hwnd, hdc);

      previousTime = st;
      return 0;

   case WM_PAINT:
      hdc = BeginPaint(hwnd, &ps);

      SetIsotropic(hdc, cxClient, cyClient);
      DrawClock(hdc);
      DrawHands(hdc, &previousTime, TRUE);

      EndPaint(hwnd, &ps);
      return 0;

   case WM_DESTROY:
      KillTimer(hwnd, ID_TIMER);
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(hwnd, message, wParam, lParam);
}