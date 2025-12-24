/*-----------------------------------------
   WHATSIZE.C -- What Size is the Window?
                 (c) Charles Petzold, 1998
  -----------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

int WINAPI wWinMain( _In_     HINSTANCE inst,
                     _In_opt_ HINSTANCE prevInst,
                     _In_     PWSTR     cmdLine,
                     _In_     int       nShowCmd )
{
   UNREFERENCED_PARAMETER( prevInst );
   UNREFERENCED_PARAMETER( cmdLine );

   static PCWSTR appName = L"WhatSize";
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

   wnd = CreateWindowW( appName, L"What Size is the Window?",
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        NULL, NULL, inst, NULL );

   ShowWindow( wnd, nShowCmd );
   UpdateWindow( wnd );

   while ( GetMessageW( &msg, NULL, 0, 0 ) )
   {
      TranslateMessage( &msg );
      DispatchMessageW( &msg );
   }
   return ( int ) msg.wParam;
}

void Show( HWND wnd, HDC dc, int xText, int yText, int mapMode, PCWSTR textMapMode )
{
   WCHAR buffer[60] = { L"" };
   RECT  rect;

   SaveDC( dc );

   SetMapMode( dc, mapMode );
   GetClientRect( wnd, &rect );
   DPtoLP( dc, ( PPOINT ) &rect, 2 );

   RestoreDC( dc, -1 );

   TextOutW( dc, xText, yText, buffer,
             wsprintfW( buffer, L"%-20s %7d %7d %7d %7d", textMapMode,
             rect.left, rect.right, rect.top, rect.bottom ) );
}

LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static PCWSTR heading   = L"Mapping Mode            Left   Right     Top  Bottom";
   static PCWSTR underline = L"------------            ----   -----     ---  ------";
   static int    xChar;
   static int    yChar;
   HDC           dc;
   PAINTSTRUCT   ps;
   TEXTMETRICW   tm;

   switch ( msg )
   {
   case WM_CREATE:
      dc = GetDC( wnd );
      SelectObject( dc, GetStockObject( SYSTEM_FIXED_FONT ) );

      GetTextMetricsW( dc, &tm );
      xChar = tm.tmAveCharWidth;
      yChar = tm.tmHeight + tm.tmExternalLeading;

      ReleaseDC( wnd, dc );
      return 0;

   case WM_PAINT:
      dc = BeginPaint( wnd, &ps );
      SelectObject( dc, GetStockObject( SYSTEM_FIXED_FONT ) );

      SetMapMode( dc, MM_ANISOTROPIC );
      SetWindowExtEx( dc, 1, 1, NULL );
      SetViewportExtEx( dc, xChar, yChar, NULL );

      TextOutW( dc, 1, 1, heading, lstrlenW( heading ) );
      TextOutW( dc, 1, 2, underline, lstrlenW( underline ) );

      Show( wnd, dc, 1, 3, MM_TEXT, L"TEXT (pixels)" );
      Show( wnd, dc, 1, 4, MM_LOMETRIC, L"LOMETRIC (.1 mm)" );
      Show( wnd, dc, 1, 5, MM_HIMETRIC, L"HIMETRIC (.01 mm)" );
      Show( wnd, dc, 1, 6, MM_LOENGLISH, L"LOENGLISH (.01 in)" );
      Show( wnd, dc, 1, 7, MM_HIENGLISH, L"HIENGLISH (.001 in)" );
      Show( wnd, dc, 1, 8, MM_TWIPS, L"TWIPS (1/1440 in)" );

      EndPaint( wnd, &ps );
      return 0;

   case WM_DESTROY:
      PostQuitMessage( 0 );
      return 0;
   }

   return DefWindowProcW( wnd, msg, wParam, lParam );
}
