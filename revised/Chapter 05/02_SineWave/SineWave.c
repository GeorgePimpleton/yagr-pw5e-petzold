/*-----------------------------------------
   SINEWAVE.C -- Sine Wave Using Polyline
                 (c) Charles Petzold, 1998
  -----------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>

#define _USE_MATH_DEFINES
#include <math.h>

#define NUM    1000
#define TWOPI  (2.0 * M_PI)

LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

int WINAPI wWinMain( _In_     HINSTANCE inst,
                     _In_opt_ HINSTANCE prevInst,
                     _In_     PWSTR     cmdLine,
                     _In_     int       showCmd )
{
   UNREFERENCED_PARAMETER( prevInst );
   UNREFERENCED_PARAMETER( cmdLine );

   static PCWSTR appName = L"SineWave";
   HWND          wnd;
   MSG           msg;
   WNDCLASSW     wc      = { 0 };

   wc.style         = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = inst;
   wc.hIcon         = ( HICON ) LoadImageW( NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR );
   wc.hCursor       = ( HCURSOR ) LoadImageW( NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED );
   wc.hbrBackground = ( HBRUSH ) ( COLOR_WINDOW + 1 );
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = appName;

   if ( !RegisterClassW( &wc ) )
   {
      MessageBoxW( NULL, L"Program requires Windows NT!",
                   appName, MB_ICONERROR );
      return 0;
   }

   wnd = CreateWindowW( appName, L"Sine Wave Using Polyline",
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        NULL, NULL, inst, NULL );

   ShowWindow( wnd, showCmd );
   UpdateWindow( wnd );

   while ( GetMessageW( &msg, NULL, 0, 0 ) )
   {
      TranslateMessage( &msg );
      DispatchMessageW( &msg );
   }
   return ( int ) msg.wParam;
}

LRESULT CALLBACK WndProc( HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
   static int  xClient;
   static int  yClient;
   HDC         dc;
   int         i;
   PAINTSTRUCT ps;
   POINT       apt[NUM] = { 0 };

   switch ( msg )
   {
   case WM_SIZE:
      xClient = GET_X_LPARAM( lParam );
      yClient = GET_Y_LPARAM( lParam );
      return 0;

   case WM_PAINT:
      dc = BeginPaint( wnd, &ps );

      MoveToEx( dc, 0, yClient / 2, NULL );
      LineTo( dc, xClient, yClient / 2 );

      for ( i = 0; i < NUM; i++ )
      {
         apt[i].x = i * xClient / NUM;
         apt[i].y = ( int ) ( yClient / 2.0 * ( 1 - sin( TWOPI * i / NUM ) ) );
      }

      Polyline( dc, apt, NUM );
      return 0;

   case WM_DESTROY:
      PostQuitMessage( 0 );
      return 0;
   }

   return DefWindowProcW( wnd, msg, wParam, lParam );
}
