/*--------------------------------------
   CLOCK.C -- Analog Clock Program
              (c) Charles Petzold, 1998
  --------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>

#define _USE_MATH_DEFINES
#include <math.h>

#define ID_TIMER 1
#define TWOPI    ( 2 * M_PI )

LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

int WINAPI wWinMain( _In_     HINSTANCE instance,
                     _In_opt_ HINSTANCE prevInstance,
                     _In_     PWSTR     cmdLine,
                     _In_     int       showCmd )
{
   UNREFERENCED_PARAMETER( prevInstance );
   UNREFERENCED_PARAMETER( cmdLine );

   static PCWSTR appName = L"Clock";
   HWND          wnd;
   MSG           msg;
   WNDCLASSW     wc      = { 0 };

   wc.style         = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = instance;
   wc.hIcon         = NULL;
   wc.hCursor       = ( HCURSOR ) LoadImageW( NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED );
   wc.hbrBackground = ( HBRUSH )  GetStockObject( WHITE_BRUSH );
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = appName;

   if ( !RegisterClassW( &wc ) )
   {
      MessageBoxW( NULL, L"Program requires Windows NT!",
                   appName, MB_ICONERROR );
      return 0;
   }

   wnd = CreateWindowW( appName, L"Analog Clock",
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        NULL, NULL, instance, NULL );

   ShowWindow( wnd, showCmd );
   UpdateWindow( wnd );

   while ( GetMessageW( &msg, NULL, 0, 0 ) )
   {
      TranslateMessage( &msg );
      DispatchMessageW( &msg );
   }
   return ( int ) msg.wParam;
}

void SetIsotropic( HDC dc, int xClient, int yClient )
{
   SetMapMode( dc, MM_ISOTROPIC );
   SetWindowExtEx( dc, 1000, 1000, NULL );
   SetViewportExtEx( dc, xClient / 2, -yClient / 2, NULL );
   SetViewportOrgEx( dc, xClient / 2, yClient / 2, NULL );
}

void RotatePoint( POINT pt[ ], int num, int angle )
{
   int   i;
   POINT temp = { 0 };

   for ( i = 0; i < num; i++ )
   {
      temp.x = ( int ) ( pt[i].x * cos( TWOPI * angle / 360 ) +
                         pt[i].y * sin( TWOPI * angle / 360 ) );

      temp.y = ( int ) ( pt[i].y * cos( TWOPI * angle / 360 ) -
                         pt[i].x * sin( TWOPI * angle / 360 ) );

      pt[i] = temp;
   }
}

void DrawClock( HDC dc )
{
   int   angle;
   POINT pt[3] = { 0 };

   for ( angle = 0; angle < 360; angle += 6 )
   {
      pt[0].x = 0;
      pt[0].y = 900;

      RotatePoint( pt, 1, angle );

      pt[2].x = pt[2].y = angle % 5 ? 33 : 100;

      pt[0].x -= pt[2].x / 2;
      pt[0].y -= pt[2].y / 2;

      pt[1].x = pt[0].x + pt[2].x;
      pt[1].y = pt[0].y + pt[2].y;

      SelectObject( dc, GetStockObject( BLACK_BRUSH ) );

      Ellipse( dc, pt[0].x, pt[0].y, pt[1].x, pt[1].y );
   }
}

void DrawHands( HDC dc, SYSTEMTIME* sysTime, BOOL change )
{
   static POINT pt[3][5] = { { { 0, -150 }, { 100, 0 }, { 0, 600 }, { -100, 0 }, { 0, -150 } },
                             { { 0, -200 }, { 50, 0 },  { 0, 800 }, { -50, 0 },  { 0, -200 } },
                             { { 0, 0 },    { 0, 0 },   { 0, 0 },   { 0, 0 },    { 0, 800 } } };
   int          i;
   int          angle[3] = { 0 };
   POINT        temp[3][5];

   angle[0] = ( sysTime->wHour * 30 ) % 360 + sysTime->wMinute / 2;
   angle[1] = sysTime->wMinute * 6;
   angle[2] = sysTime->wSecond * 6;

   memcpy( temp, pt, sizeof( pt ) );

   for ( i = change ? 0 : 2; i < 3; i++ )
   {
      RotatePoint( temp[i], 5, angle[i] );

      Polyline( dc, temp[i], 5 );
   }
}

LRESULT CALLBACK WndProc( HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
   static int        xClient;
   static int        yClient;
   static SYSTEMTIME previousTime;
   BOOL              change;
   HDC               dc;
   PAINTSTRUCT       ps;
   SYSTEMTIME        sysTime;

   switch ( msg )
   {
   case WM_CREATE:
      SetTimer( wnd, ID_TIMER, 1000, NULL );
      GetLocalTime( &sysTime );
      previousTime = sysTime;
      return 0;

   case WM_SIZE:
      xClient = GET_X_LPARAM( lParam );
      yClient = GET_Y_LPARAM( lParam );
      return 0;

   case WM_TIMER:
      GetLocalTime( &sysTime );

      change = sysTime.wHour != previousTime.wHour ||
         sysTime.wMinute != previousTime.wMinute;

      dc = GetDC( wnd );

      SetIsotropic( dc, xClient, yClient );

      SelectObject( dc, GetStockObject( WHITE_PEN ) );
      DrawHands( dc, &previousTime, change );

      SelectObject( dc, GetStockObject( BLACK_PEN ) );
      DrawHands( dc, &sysTime, TRUE );

      ReleaseDC( wnd, dc );

      previousTime = sysTime;
      return 0;

   case WM_PAINT:
      dc = BeginPaint( wnd, &ps );

      SetIsotropic( dc, xClient, yClient );
      DrawClock( dc );
      DrawHands( dc, &previousTime, TRUE );

      EndPaint( wnd, &ps );
      return 0;

   case WM_DESTROY:
      KillTimer( wnd, ID_TIMER );
      PostQuitMessage( 0 );
      return 0;
   }

   return DefWindowProcW( wnd, msg, wParam, lParam );
}
