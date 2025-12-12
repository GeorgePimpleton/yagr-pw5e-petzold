/*--------------------------------------------------
   CLOVER.C -- Clover Drawing Program Using Regions
               (c) Charles Petzold, 1998
  --------------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>

#define _USE_MATH_DEFINES
#include <math.h>

#define TWO_PI (2.0 * M_PI)

LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

int WINAPI wWinMain( _In_     HINSTANCE inst,
                     _In_opt_ HINSTANCE prevInst,
                     _In_     PWSTR     cmdLine,
                     _In_     int       showCmd )
{
   UNREFERENCED_PARAMETER( prevInst );
   UNREFERENCED_PARAMETER( cmdLine );

   static PCWSTR appName = L"Clover";
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
      MessageBoxW( NULL, L"This program requires Windows NT!",
                   appName, MB_ICONERROR );
      return 0;
   }

   wnd = CreateWindowW( appName, L"Draw a Clover",
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
   static HRGN rgnClip;
   static int  xClient;
   static int  yClient;
   double      angle;
   double      radius;
   HCURSOR     cursor;
   HDC         dc;
   HRGN        rgnTemp[6] = { 0 };
   int         i;
   PAINTSTRUCT ps;

   switch ( msg )
   {
   case WM_SIZE:
      xClient = GET_X_LPARAM( lParam );
      yClient = GET_Y_LPARAM( lParam );

      cursor = SetCursor( ( HCURSOR ) LoadImageW( NULL, IDC_WAIT, IMAGE_CURSOR, 0, 0, LR_SHARED ) );
      ShowCursor( TRUE );

      if ( rgnClip )
      {
         DeleteObject( rgnClip );
      }

      rgnTemp[0] = CreateEllipticRgn( 0, yClient / 3, xClient / 2, 2 * yClient / 3 );
      rgnTemp[1] = CreateEllipticRgn( xClient / 2, yClient / 3, xClient, 2 * yClient / 3 );
      rgnTemp[2] = CreateEllipticRgn( xClient / 3, 0, 2 * xClient / 3, yClient / 2 );
      rgnTemp[3] = CreateEllipticRgn( xClient / 3, yClient / 2, 2 * xClient / 3, yClient );
      rgnTemp[4] = CreateRectRgn( 0, 0, 1, 1 );
      rgnTemp[5] = CreateRectRgn( 0, 0, 1, 1 );
      rgnClip    = CreateRectRgn( 0, 0, 1, 1 );

      CombineRgn( rgnTemp[4], rgnTemp[0], rgnTemp[1], RGN_OR );
      CombineRgn( rgnTemp[5], rgnTemp[2], rgnTemp[3], RGN_OR );
      CombineRgn( rgnClip, rgnTemp[4], rgnTemp[5], RGN_XOR );

      for ( i = 0; i < 6; i++ )
      {
         DeleteObject( rgnTemp[i] );
      }

      SetCursor( cursor );
      ShowCursor( FALSE );
      return 0;

   case WM_PAINT:
      dc = BeginPaint( wnd, &ps );

      SetViewportOrgEx( dc, xClient / 2, yClient / 2, NULL );
      SelectClipRgn( dc, rgnClip );

      radius = _hypot( xClient / 2.0, yClient / 2.0 );

      for ( angle = 0.0; angle < TWO_PI; angle += TWO_PI / 360 )
      {
         MoveToEx( dc, 0, 0, NULL );
         LineTo( dc, ( int ) ( radius * cos( angle ) + 0.5 ),
                 ( int ) ( -radius * sin( angle ) + 0.5 ) );
      }
      EndPaint( wnd, &ps );
      return 0;

   case WM_DESTROY: DeleteObject( rgnClip );
      PostQuitMessage( 0 );
      return 0;
   }

   return DefWindowProcW( wnd, msg, wParam, lParam );
}
