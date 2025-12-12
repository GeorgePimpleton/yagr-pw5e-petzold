/*-----------------------------------------
   STOKFONT.C -- Stock Font Objects
                 (c) Charles Petzold, 1998
  -----------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdlib.h>

LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

int WINAPI wWinMain( _In_     HINSTANCE inst,
                     _In_opt_ HINSTANCE prevInst,
                     _In_     PWSTR     cmdLine,
                     _In_     int       showCmd )
{
   UNREFERENCED_PARAMETER( prevInst );
   UNREFERENCED_PARAMETER( cmdLine );

   static PCWSTR appName = L"StokFont";
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

   wnd = CreateWindowW( appName, L"Stock Fonts",
                        WS_OVERLAPPEDWINDOW | WS_VSCROLL,
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
   static struct
   {
      int    idStockFont;
      PCWSTR stockFont;
   }
   stockfont[ ] = { { OEM_FIXED_FONT,       L"OEM_FIXED_FONT"     },
                    { ANSI_FIXED_FONT,     L"ANSI_FIXED_FONT"     },
                    { ANSI_VAR_FONT,       L"ANSI_VAR_FONT"       },
                    { SYSTEM_FONT,         L"SYSTEM_FONT"         },
                    { DEVICE_DEFAULT_FONT, L"DEVICE_DEFAULT_FONT" },
                    { SYSTEM_FIXED_FONT,   L"SYSTEM_FIXED_FONT"   },
                    { DEFAULT_GUI_FONT,    L"DEFAULT_GUI_FONT"    } };

   static int  font;
   static int  numFonts = _countof( stockfont );
   HDC         dc;
   int         i;
   int         x;
   int         y;
   int         xGrid;
   int         yGrid;
   PAINTSTRUCT ps;
   WCHAR       faceName[LF_FACESIZE];
   WCHAR       buffer[LF_FACESIZE + 64] = { L"" };
   TEXTMETRICW tm;

   switch ( msg )
   {
   case WM_CREATE:
      SetScrollRange( wnd, SB_VERT, 0, numFonts - 1, TRUE );
      return 0;

   case WM_DISPLAYCHANGE:
      InvalidateRect( wnd, NULL, TRUE );
      return 0;

   case WM_VSCROLL:
      switch ( LOWORD( wParam ) )
      {
      case SB_TOP:
         font = 0;
         break;

      case SB_BOTTOM:
         font = numFonts - 1;
         break;

      case SB_LINEUP:
      case SB_PAGEUP:
         font -= 1;
         break;

      case SB_LINEDOWN:
      case SB_PAGEDOWN:
         font += 1;
         break;

      case SB_THUMBPOSITION:
         font = HIWORD( wParam );
         break;
      }

      font = max( 0, min( numFonts - 1, font ) );
      SetScrollPos( wnd, SB_VERT, font, TRUE );
      InvalidateRect( wnd, NULL, TRUE );
      return 0;

   case WM_KEYDOWN:
      switch ( wParam )
      {
      case VK_HOME:
         SendMessageW( wnd, WM_VSCROLL, SB_TOP, 0 );
         break;

      case VK_END:
         SendMessageW( wnd, WM_VSCROLL, SB_BOTTOM, 0 );
         break;

      case VK_PRIOR:
      case VK_LEFT:
      case VK_UP:
         SendMessageW( wnd, WM_VSCROLL, SB_LINEUP, 0 );
         break;

      case VK_NEXT:
      case VK_RIGHT:
      case VK_DOWN:
         SendMessageW( wnd, WM_VSCROLL, SB_PAGEDOWN, 0 );
         break;
      }
      return 0;

   case WM_PAINT:
      dc = BeginPaint( wnd, &ps );

      SelectObject( dc, GetStockObject( stockfont[font].idStockFont ) );
      GetTextFaceW( dc, LF_FACESIZE, faceName );
      GetTextMetricsW( dc, &tm );

      xGrid = max( 3 * tm.tmAveCharWidth, 2 * tm.tmMaxCharWidth );
      yGrid = tm.tmHeight + 3;

      TextOutW( dc, 0, 0, buffer,
                wsprintfW( buffer, L" %s: Face Name = %s, CharSet = %i",
                stockfont[font].stockFont,
                faceName, tm.tmCharSet ) );

      SetTextAlign( dc, TA_TOP | TA_CENTER );

      // vertical and horizontal lines
      for ( i = 0; i < 17; i++ )
      {
         MoveToEx( dc, ( i + 2 ) * xGrid, 2 * yGrid, NULL );
         LineTo( dc, ( i + 2 ) * xGrid, 19 * yGrid );

         MoveToEx( dc, xGrid, ( i + 3 ) * yGrid, NULL );
         LineTo( dc, 18 * xGrid, ( i + 3 ) * yGrid );
      }

      // vertical and horizontal headings
      for ( i = 0; i < 16; i++ )
      {
         TextOutW( dc, ( 2 * i + 5 ) * xGrid / 2, 2 * yGrid + 2, buffer,
                   wsprintfW( buffer, L"%X-", i ) );

         TextOutW( dc, 3 * xGrid / 2, ( i + 3 ) * yGrid + 2, buffer,
                   wsprintfW( buffer, L"-%X", i ) );
      }

      // characters
      for ( y = 0; y < 16; y++ )
      {
         for ( x = 0; x < 16; x++ )
         {
            TextOutW( dc, ( 2 * x + 5 ) * xGrid / 2,
                      ( y + 3 ) * yGrid + 2, buffer,
                      wsprintfW( buffer, L"%c", 16 * x + y ) );
         }
      }

      EndPaint( wnd, &ps );
      return 0;

   case WM_DESTROY:
      PostQuitMessage( 0 );
      return 0;
   }

   return DefWindowProcW( wnd, msg, wParam, lParam );
}
