/*--------------------------------------------------
   CONNECT.C -- Connect-the-Dots Mouse Demo Program
                (c) Charles Petzold, 1998
  --------------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#define MAXPOINTS 1000

LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

int WINAPI wWinMain( _In_     HINSTANCE inst,
                     _In_opt_ HINSTANCE prevInst,
                     _In_     PWSTR     cmdLine,
                     _In_     int       showCmd )
{
   UNREFERENCED_PARAMETER( prevInst );
   UNREFERENCED_PARAMETER( cmdLine );

   static PCWSTR appName = L"Connect";
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
      MessageBoxW( NULL, L"Program requires Windows NT!",
                   appName, MB_ICONERROR );
      return 0;
   }

   wnd = CreateWindowW( appName, L"Connect-the-Points Mouse Demo",
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
   static POINT pt[MAXPOINTS];
   static int   count;
   HDC          dc;
   int          i;
   int          j;
   PAINTSTRUCT  ps;

   switch ( msg )
   {
   case WM_LBUTTONDOWN:
      count = 0;
      InvalidateRect( wnd, NULL, TRUE );
      return 0;

   case WM_MOUSEMOVE:
      if ( wParam & MK_LBUTTON && count < MAXPOINTS )
      {
         pt[count].x   = LOWORD( lParam );
         pt[count++].y = HIWORD( lParam );

         dc = GetDC( wnd );
         SetPixel( dc, LOWORD( lParam ), HIWORD( lParam ), 0 );
         ReleaseDC( wnd, dc );
      }
      return 0;

   case WM_LBUTTONUP:
      InvalidateRect( wnd, NULL, FALSE );
      return 0;

   case WM_PAINT:
      dc = BeginPaint( wnd, &ps );

      SetCursor( LoadCursorW( NULL, IDC_WAIT ) );
      ShowCursor( TRUE );

      for ( i = 0; i < count - 1; i++ )
      {
         for ( j = i + 1; j < count; j++ )
         {
            MoveToEx( dc, pt[i].x, pt[i].y, NULL );
            LineTo( dc, pt[j].x, pt[j].y );
         }
         Sleep( 1 );
      }

      ShowCursor( FALSE );
      SetCursor( LoadCursorW( NULL, IDC_ARROW ) );

      EndPaint( wnd, &ps );
      return 0;

   case WM_DESTROY:
      PostQuitMessage( 0 );
      return 0;
   }

   return DefWindowProcW( wnd, msg, wParam, lParam );
}
