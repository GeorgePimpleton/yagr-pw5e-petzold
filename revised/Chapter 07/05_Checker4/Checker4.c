/*-------------------------------------------------
   CHECKER4.C -- Mouse Hit-Test Demo Program No. 4
                 (c) Charles Petzold, 1998
  -------------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#define DIVISIONS 5

LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK ChildWndProc( HWND, UINT, WPARAM, LPARAM );

int    idFocus = 0;
PCWSTR childClass = L"Checker4_Child";

int WINAPI wWinMain( _In_     HINSTANCE inst,
                     _In_opt_ HINSTANCE prevInst,
                     _In_     PTSTR     cmdLine,
                     _In_     int       showCmd )
{
   UNREFERENCED_PARAMETER( prevInst );
   UNREFERENCED_PARAMETER( cmdLine );

   static PCWSTR appName = L"Checker4";
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

   wc.lpfnWndProc = ChildWndProc;
   wc.cbWndExtra = sizeof( long );
   wc.hIcon = NULL;
   wc.lpszClassName = childClass;

   RegisterClassW( &wc );

   wnd = CreateWindowW( appName, L"Checker4 Mouse Hit-Test Demo",
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
   static HWND wndChild[DIVISIONS][DIVISIONS];
   int         xBlock;
   int         yBlock;
   int         x;
   int         y;

   switch ( msg )
   {
   case WM_CREATE:
      for ( x = 0; x < DIVISIONS; x++ )
      {
         for ( y = 0; y < DIVISIONS; y++ )
         {
            wndChild[x][y] = CreateWindowW( childClass, NULL,
                                            WS_CHILDWINDOW | WS_VISIBLE,
                                            0, 0, 0, 0,
                                            wnd, ( HMENU ) ( y << 8 | x ),
                                            ( HINSTANCE ) GetWindowLongPtrW( wnd, GWLP_HINSTANCE ),
                                            NULL );
         }
      }
      return 0;

   case WM_SIZE:
      xBlock = LOWORD( lParam ) / DIVISIONS;
      yBlock = HIWORD( lParam ) / DIVISIONS;

      for ( x = 0; x < DIVISIONS; x++ )
      {
         for ( y = 0; y < DIVISIONS; y++ )
         {
            MoveWindow( wndChild[x][y],
                        x * xBlock, y * yBlock,
                        xBlock, yBlock, TRUE );
         }
      }
      return 0;

   case WM_LBUTTONDOWN:
      MessageBeep( 0 );
      return 0;

      // on set-focus message, set focus to child window
   case WM_SETFOCUS:
      SetFocus( GetDlgItem( wnd, idFocus ) );
      return 0;

      // on key-down message, possibly change the focus window
   case WM_KEYDOWN:
      x = idFocus & 0xFF;
      y = idFocus >> 8;

      switch ( wParam )
      {
      case VK_UP:    y--;                    break;
      case VK_DOWN:  y++;                    break;
      case VK_LEFT:  x--;                    break;
      case VK_RIGHT: x++;                    break;
      case VK_HOME:  x = y = 0;              break;
      case VK_END:   x = y = DIVISIONS - 1;  break;
      default:       return 0;
      }

      x = ( x + DIVISIONS ) % DIVISIONS;
      y = ( y + DIVISIONS ) % DIVISIONS;

      idFocus = y << 8 | x;

      SetFocus( GetDlgItem( wnd, idFocus ) );
      return 0;

   case WM_DESTROY:
      PostQuitMessage( 0 );
      return 0;
   }

   return DefWindowProcW( wnd, msg, wParam, lParam );
}

LRESULT CALLBACK ChildWndProc( HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
   HDC         dc;
   PAINTSTRUCT ps;
   RECT        rect;

   switch ( msg )
   {
   case WM_CREATE:
      SetWindowLongPtrW( wnd, GWLP_USERDATA, 0 );       // on/off flag
      return 0;

   case WM_KEYDOWN:
      // send most key presses to the parent window
      if ( wParam != VK_RETURN && wParam != VK_SPACE )
      {
         SendMessageW( GetParent( wnd ), msg, wParam, lParam );
         return 0;
      }
      // For Return and Space, to toggle the square
      // falls through

   case WM_LBUTTONDOWN:
      SetWindowLongPtrW( wnd, GWLP_USERDATA, 1 ^ GetWindowLongPtr( wnd, GWLP_USERDATA ) );
      SetFocus( wnd );
      InvalidateRect( wnd, NULL, FALSE );
      return 0;

      // for focus messages, invalidate the window for repaint

   case WM_SETFOCUS:
      idFocus = ( int ) GetWindowLongPtrW( wnd, GWLP_ID );

      // fall through

   case WM_KILLFOCUS:
      InvalidateRect( wnd, NULL, TRUE );
      return 0;

   case WM_PAINT:
      dc = BeginPaint( wnd, &ps );

      GetClientRect( wnd, &rect );
      Rectangle( dc, 0, 0, rect.right, rect.bottom );

      // draw the "x" mark
      if ( GetWindowLongPtrW( wnd, GWLP_USERDATA ) )
      {
         MoveToEx( dc, 0, 0, NULL );
         LineTo( dc, rect.right, rect.bottom );
         MoveToEx( dc, 0, rect.bottom, NULL );
         LineTo( dc, rect.right, 0 );
      }

      // draw the "focus" rectangle
      if ( wnd == GetFocus( ) )
      {
         rect.left   += rect.right / 10;
         rect.right  -= rect.left;
         rect.top    += rect.bottom / 10;
         rect.bottom -= rect.top;

         SelectObject( dc, GetStockObject( NULL_BRUSH ) );
         SelectObject( dc, CreatePen( PS_DASH, 0, 0 ) );
         Rectangle( dc, rect.left, rect.top, rect.right, rect.bottom );
         DeleteObject( SelectObject( dc, GetStockObject( BLACK_PEN ) ) );
      }

      EndPaint( wnd, &ps );
      return 0;
   }

   return DefWindowProcW( wnd, msg, wParam, lParam );
}
