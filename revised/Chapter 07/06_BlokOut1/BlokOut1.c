/*-----------------------------------------
   BLOKOUT1.C -- Mouse Button Demo Program
                 (c) Charles Petzold, 1998
  -----------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

int WINAPI wWinMain( _In_     HINSTANCE inst,
                     _In_opt_ HINSTANCE prevInst,
                     _In_     PWSTR     cmdLine,
                     _In_     int       showCmd )
{
   UNREFERENCED_PARAMETER( prevInst );
   UNREFERENCED_PARAMETER( cmdLine );

   static PCWSTR appName = L"BlokOut1";
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
   wc.hbrBackground = ( HBRUSH )  GetStockObject( WHITE_BRUSH );
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = appName;

   if ( !RegisterClassW( &wc ) )
   {
      MessageBoxW( NULL, L"Program requires Windows NT!",
                   appName, MB_ICONERROR );
      return 0;
   }

   wnd = CreateWindowW( appName, L"Mouse Button Demo",
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

void DrawBoxOutline( HWND wnd, POINT beg, POINT end )
{
   HDC dc = GetDC( wnd );

   SetROP2( dc, R2_NOT );
   SelectObject( dc, GetStockObject( NULL_BRUSH ) );
   Rectangle( dc, beg.x, beg.y, end.x, end.y );

   ReleaseDC( wnd, dc );
}

LRESULT CALLBACK WndProc( HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
   static BOOL  blocking;
   static BOOL  validBox;
   static POINT beg;
   static POINT end;
   static POINT boxBeg;
   static POINT boxEnd;
   HDC          dc;
   PAINTSTRUCT  ps;

   switch ( msg )
   {
   case WM_LBUTTONDOWN:
      beg.x = end.x = LOWORD( lParam );
      beg.y = end.y = HIWORD( lParam );

      DrawBoxOutline( wnd, beg, end );

      SetCursor( ( HCURSOR ) LoadImageW( NULL, IDC_CROSS, IMAGE_CURSOR, 0, 0, LR_SHARED ) );

      blocking = TRUE;
      return 0;

   case WM_MOUSEMOVE:
      if ( blocking )
      {
         SetCursor( ( HCURSOR ) LoadImageW( NULL, IDC_CROSS, IMAGE_CURSOR, 0, 0, LR_SHARED ) );

         DrawBoxOutline( wnd, beg, end );

         end.x = LOWORD( lParam );
         end.y = HIWORD( lParam );

         DrawBoxOutline( wnd, beg, end );
      }
      return 0;

   case WM_LBUTTONUP:
      if ( blocking )
      {
         DrawBoxOutline( wnd, beg, end );

         boxBeg   = beg;
         boxEnd.x = LOWORD( lParam );
         boxEnd.y = HIWORD( lParam );

         SetCursor( ( HCURSOR ) LoadImageW( NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED ) );

         blocking = FALSE;
         validBox = TRUE;

         InvalidateRect( wnd, NULL, TRUE );
      }
      return 0;

   case WM_CHAR:
      if ( blocking && ( wParam == '\x1B' ) )     // i.e., Escape
      {
         DrawBoxOutline( wnd, beg, end );

         SetCursor( ( HCURSOR ) LoadImageW( NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED ) );

         blocking = FALSE;
      }
      return 0;

   case WM_PAINT:
      dc = BeginPaint( wnd, &ps );

      if ( validBox )
      {
         SelectObject( dc, GetStockObject( BLACK_BRUSH ) );
         Rectangle( dc, boxBeg.x, boxBeg.y, boxEnd.x, boxEnd.y );
      }

      if ( blocking )
      {
         SetROP2( dc, R2_NOT );
         SelectObject( dc, GetStockObject( NULL_BRUSH ) );
         Rectangle( dc, beg.x, beg.y, end.x, end.y );
      }

      EndPaint( wnd, &ps );
      return 0;

   case WM_DESTROY:
      PostQuitMessage( 0 );
      return 0;
   }

   return DefWindowProcW( wnd, msg, wParam, lParam );
}
