/*-------------------------------------------------------
   POPPAD1.C -- Popup Editor using child window edit box
                (c) Charles Petzold, 1998
  -------------------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>

#define ID_EDIT     1

LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

PCWSTR g_appName = L"PopPad1";

int WINAPI wWinMain( _In_     HINSTANCE inst,
                     _In_opt_ HINSTANCE prevInst,
                     _In_     PWSTR     cmdLine,
                     _In_     int       showCmd )
{
   UNREFERENCED_PARAMETER( prevInst );
   UNREFERENCED_PARAMETER( cmdLine );

   HWND      wnd;
   MSG       msg;
   WNDCLASSW wc = { 0 };

   wc.style         = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = inst;
   wc.hIcon         = ( HICON )   LoadImageW( NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR );
   wc.hCursor       = ( HCURSOR ) LoadImageW( NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED );
   wc.hbrBackground = ( HBRUSH ) ( COLOR_WINDOW + 1 );
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = g_appName;

   if ( !RegisterClassW( &wc ) )
   {
      MessageBoxW( NULL, L"This program requires Windows NT!", g_appName, MB_ICONERROR );
      return 0;
   }

   wnd = CreateWindowW( g_appName, g_appName,
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
   static HWND edit;

   switch ( msg )
   {
   case WM_CREATE:
      edit = CreateWindowW( L"edit", NULL,
                            WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | WS_BORDER |
                            ES_LEFT | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
                            0, 0, 0, 0, wnd, ( HMENU ) ID_EDIT,
                            ( ( LPCREATESTRUCTW ) lParam )->hInstance, NULL );
      return 0;

   case WM_SETFOCUS:
      SetFocus( edit );
      return 0;

   case WM_SIZE:
      MoveWindow( edit, 0, 0, LOWORD( lParam ), HIWORD( lParam ), TRUE );
      return 0;

   case WM_COMMAND:
      if ( LOWORD( wParam ) == ID_EDIT )
      {
         if ( HIWORD( wParam ) == EN_ERRSPACE ||
              HIWORD( wParam ) == EN_MAXTEXT )
         {
            MessageBoxW( wnd, L"Edit control out of space.", g_appName, MB_OK | MB_ICONSTOP );
         }
      }
      return 0;

   case WM_DESTROY:
      PostQuitMessage( 0 );
      return 0;
   }

   return DefWindowProcW( wnd, msg, wParam, lParam );
}
