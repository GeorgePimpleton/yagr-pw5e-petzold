/*-----------------------------------------------
   ALTWIND.C -- Alternate and Winding Fill Modes
               (c) Charles Petzold, 1998
  -----------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>

LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

int WINAPI wWinMain( _In_     HINSTANCE inst,
                     _In_opt_ HINSTANCE prevInst,
                     _In_     PWSTR     cmdLine,
                     _In_     int       showCmd )
{
   UNREFERENCED_PARAMETER( prevInst );
   UNREFERENCED_PARAMETER( cmdLine );

   static PCWSTR appName = L"AltWind";
   HWND          wnd;
   MSG           msg;
   WNDCLASSW     wc      = { 0 };

   wc.style         = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = inst;
   wc.hIcon         = ( HICON )   LoadImageW( NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR );
   wc.hCursor       = ( HCURSOR ) LoadImageW( NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED );
   wc.hbrBackground = ( HBRUSH ) GetStockObject( WHITE_BRUSH );
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = appName;

   if ( !RegisterClassW( &wc ) )
   {
      MessageBoxW( NULL, L"Program requires Windows NT!",
                   appName, MB_ICONERROR );
      return 0;
   }

   wnd = CreateWindowW( appName, L"Alternate and Winding Fill Modes",
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
   static POINT aptFigure[10] = { { 10, 70 }, { 50, 70 }, { 50, 10 }, { 90, 10 }, { 90, 50 },
                                  { 30, 50 }, { 30, 90 }, { 70, 90 }, { 70, 30 }, { 10, 30 } };
   static int   xClient;
   static int   yClient;
   HDC          dc;
   int          i;
   PAINTSTRUCT  ps;
   POINT        apt[10]       = { 0 };

   switch ( msg )
   {
   case WM_SIZE:
      xClient = GET_X_LPARAM( lParam );
      yClient = GET_Y_LPARAM( lParam );
      return 0;

   case WM_PAINT:
      dc = BeginPaint( wnd, &ps );

      SelectObject( dc, GetStockObject( LTGRAY_BRUSH ) );

      for ( i = 0; i < 10; i++ )
      {
         apt[i].x = xClient * aptFigure[i].x / 200;
         apt[i].y = yClient * aptFigure[i].y / 100;
      }

      SetPolyFillMode( dc, ALTERNATE );
      Polygon( dc, apt, 10 );

      for ( i = 0; i < 10; i++ )
      {
         apt[i].x += xClient / 2;
      }

      SetPolyFillMode( dc, WINDING );
      Polygon( dc, apt, 10 );

      EndPaint( wnd, &ps );
      return 0;

   case WM_DESTROY:
      PostQuitMessage( 0 );
      return 0;
   }
   return DefWindowProcW( wnd, msg, wParam, lParam );
}
