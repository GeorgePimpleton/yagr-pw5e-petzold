/*------------------------------------------
   RANDRECT.C -- Displays Random Rectangles
                 (c) Charles Petzold, 1998
  ------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>     // for the rand function

LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );
void DrawRectangle( HWND );

int xClient;
int yClient;

int WINAPI wWinMain( _In_     HINSTANCE inst,
                     _In_opt_ HINSTANCE prevInst,
                     _In_     PWSTR     cmdLine,
                     _In_     int       showCmd )
{
   UNREFERENCED_PARAMETER( prevInst );
   UNREFERENCED_PARAMETER( cmdLine );

   static PCWSTR appName = L"RandRect";
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
   wc.hbrBackground = ( HBRUSH ) ( COLOR_WINDOW + 1 );
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = appName;

   if ( !RegisterClassW( &wc ) )
   {
      MessageBoxW( NULL, L"This program requires Windows NT!",
                   appName, MB_ICONERROR );
      return 0;
   }

   wnd = CreateWindowW( appName, L"Random Rectangles",
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        NULL, NULL, inst, NULL );

   ShowWindow( wnd, showCmd );
   UpdateWindow( wnd );

   while ( TRUE )
   {
      if ( PeekMessageW( &msg, NULL, 0, 0, PM_REMOVE ) )
      {
         if ( msg.message == WM_QUIT )
         {
            break;
         }

         TranslateMessage( &msg );
         DispatchMessageW( &msg );
      }
      else
      {
         DrawRectangle( wnd );

         // slow down the rect drawing to make it noticeable
         // but not enough to make Windows sluggish
         Sleep( 10 );
      }
   }

   return ( int ) msg.wParam;
}

LRESULT CALLBACK WndProc( HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
   switch ( msg )
   {
   case WM_SIZE:
      xClient = GET_X_LPARAM( lParam );
      yClient = GET_Y_LPARAM( lParam );
      return 0;

   case WM_DESTROY:
      PostQuitMessage( 0 );
      return 0;
   }

   return DefWindowProcW( wnd, msg, wParam, lParam );
}

void DrawRectangle( HWND wnd )
{
   HBRUSH brush;
   HDC    dc;
   RECT   rect;

   if ( xClient == 0 || yClient == 0 )
   {
      return;
   }

   SetRect( &rect, rand( ) % xClient, rand( ) % yClient,
                   rand( ) % xClient, rand( ) % yClient );

   brush = CreateSolidBrush( RGB( rand( ) % 256, rand( ) % 256, rand( ) % 256 ) );

   dc = GetDC( wnd );

   FillRect( dc, &rect, brush );
   ReleaseDC( wnd, dc );
   DeleteObject( brush );
}
