/*----------------------------------------
   BEEPER2.C -- Timer Demo Program No. 2
                (c) Charles Petzold, 1998
  ----------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#define ID_TIMER 1

LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );
VOID    CALLBACK TimerProc( HWND, UINT, UINT_PTR, DWORD );

int WINAPI wWinMain( _In_     HINSTANCE inst,
                     _In_opt_ HINSTANCE prevInst,
                     _In_     PWSTR     cmdLine,
                     _In_     int       showCmd )
{
   UNREFERENCED_PARAMETER( prevInst );
   UNREFERENCED_PARAMETER( cmdLine );

   static PCWSTR appName = L"Beeper2";
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
      MessageBoxW( NULL, L"Program requires Windows NT!", appName, MB_ICONERROR );
      return 0;
   }

   wnd = CreateWindowW( appName, L"Beeper2 Timer Demo",
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
   switch ( msg )
   {
   case WM_CREATE:
      SetTimer( wnd, ID_TIMER, 1000, TimerProc );
      return 0;

   case WM_DESTROY:
      KillTimer( wnd, ID_TIMER );
      PostQuitMessage( 0 );
      return 0;
   }

   return DefWindowProcW( wnd, msg, wParam, lParam );
}

VOID CALLBACK TimerProc( HWND wnd, UINT msg, UINT_PTR timerID, DWORD time )
{
   UNREFERENCED_PARAMETER( msg );
   UNREFERENCED_PARAMETER( timerID );
   UNREFERENCED_PARAMETER( time );

   static BOOL flipFlop = FALSE;
   HBRUSH      brush;
   HDC         dc;
   RECT        rc;

   MessageBeep( -1 );
   flipFlop = !flipFlop;

   GetClientRect( wnd, &rc );

   dc    = GetDC( wnd );
   brush = CreateSolidBrush( flipFlop ? RGB( 255, 0, 0 ) : RGB( 0, 0, 255 ) );

   FillRect( dc, &rc, brush );
   ReleaseDC( wnd, dc );
   DeleteObject( brush );
}
