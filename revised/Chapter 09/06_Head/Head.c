/*---------------------------------------------
   HEAD.C -- Displays beginning (head) of file
             (c) Charles Petzold, 1998
  ---------------------------------------------*/

#define WIN32_LEAN_AND_MEAN
#undef NOUSER

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#pragma comment(lib, "comctl32.lib")

#define ID_LIST  1
#define ID_TEXT  2

#define MAXREAD  8192
#define DIRATTR  (DDL_READWRITE | DDL_READONLY | DDL_HIDDEN | DDL_SYSTEM | \
                 DDL_DIRECTORY | DDL_ARCHIVE  | DDL_DRIVES)
#define DTFLAGS  (DT_WORDBREAK | DT_EXPANDTABS | DT_NOCLIP | DT_NOPREFIX)

LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK ListProc( HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR );

int WINAPI wWinMain( _In_     HINSTANCE inst,
                     _In_opt_ HINSTANCE prevInst,
                     _In_     PWSTR     cmdLine,
                     _In_     int       showCmd )
{
   UNREFERENCED_PARAMETER( prevInst );
   UNREFERENCED_PARAMETER( cmdLine );

   static PCWSTR appName = L"head";
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
   wc.hbrBackground = ( HBRUSH ) ( COLOR_BTNFACE + 1 );
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = appName;

   if ( !RegisterClassW( &wc ) )
   {
      MessageBoxW( NULL, L"This program requires Windows NT!", appName, MB_ICONERROR );
      return 0;
   }

   wnd = CreateWindowW( appName, L"head",
                        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
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
   static BOOL  validFile;
   static char  buffer[MAXREAD];
   static HWND  list;
   static HWND  text;
   static RECT  rect;
   static WCHAR fileLoc[MAX_PATH + 1];
   HANDLE       file;
   HDC          dc;
   int          xChar;
   int          yChar;
   DWORD        bytesRead;
   PAINTSTRUCT  ps;
   WCHAR        Buffer[MAX_PATH + 1];

   switch ( msg )
   {
   case WM_CREATE:
      xChar = LOWORD( GetDialogBaseUnits( ) );
      yChar = HIWORD( GetDialogBaseUnits( ) );

      rect.left = 20 * xChar;
      rect.top = 3 * yChar;

      list = CreateWindowExW( 0, L"listbox", NULL,
                              WS_CHILDWINDOW | WS_VISIBLE | LBS_STANDARD,
                              xChar, yChar * 3,
                              xChar * 13 + GetSystemMetrics( SM_CXVSCROLL ),
                              yChar * 10,
                              wnd, ( HMENU ) ID_LIST,
                              GetWindowInstance( wnd ),
                              NULL );

      GetCurrentDirectoryW( MAX_PATH + 1, Buffer );

      text = CreateWindowExW( 0, L"static", Buffer,
                              WS_CHILDWINDOW | WS_VISIBLE | SS_LEFT,
                              xChar, yChar, xChar * MAX_PATH, yChar,
                              wnd, ( HMENU ) ID_TEXT,
                              GetWindowInstance( wnd ),
                              NULL );

      SetWindowSubclass( list, ListProc, ID_LIST, ( DWORD_PTR ) 0 );

      ListBox_Dir( list, DIRATTR, L"*.*" );

      return 0;

   case WM_SIZE:
      rect.right  = GET_X_LPARAM( lParam );
      rect.bottom = GET_Y_LPARAM( lParam );
      return 0;

   case WM_SETFOCUS:
      SetFocus( list );
      return 0;

   case WM_COMMAND:
      if ( GET_WM_COMMAND_ID( wParam, lParam ) == ID_LIST && GET_WM_COMMAND_CMD( wParam, lParam ) == LBN_DBLCLK )
      {
         int i = ListBox_GetCurSel( list );

         if ( LB_ERR == i )
         {
            break;
         }

         ListBox_GetText( list, i, Buffer );

         file = CreateFileW( Buffer, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );

         if ( INVALID_HANDLE_VALUE != file )
         {
            CloseHandle( file );
            validFile = TRUE;
            lstrcpyW( fileLoc, Buffer );
            GetCurrentDirectoryW( MAX_PATH + 1, Buffer );

            if ( Buffer[lstrlenW( Buffer ) - 1] != '\\' )
            {
               lstrcatW( Buffer, L"\\" );
            }

            SetWindowTextW( text, lstrcatW( Buffer, fileLoc ) );
         }
         else
         {
            validFile = FALSE;
            Buffer[lstrlenW( Buffer ) - 1] = '\0';

            // If setting the directory doesn't work, maybe it's
            // a drive change, so try that.
            if ( !SetCurrentDirectoryW( Buffer + 1 ) )
            {
               Buffer[3] = ':';
               Buffer[4] = '\0';
               SetCurrentDirectoryW( Buffer + 2 );
            }

            // get the new directory name and fill the list box.
            GetCurrentDirectoryW( MAX_PATH + 1, Buffer );
            SetWindowTextW( text, Buffer );
            ListBox_ResetContent( list );
            ListBox_Dir( list, DIRATTR, L"*.*" );
         }
         InvalidateRect( wnd, NULL, TRUE );
      }
      return 0;

   case WM_PAINT:
      if ( !validFile )
      {
         break;
      }

      file = CreateFileW( fileLoc, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );

      if ( INVALID_HANDLE_VALUE == file )
      {
         validFile = FALSE;
         break;
      }

      ReadFile( file, buffer, MAXREAD, &bytesRead, NULL );
      CloseHandle( file );

      // i now equals the number of bytes in buffer.
      // commence getting a device context for displaying text.
      dc = BeginPaint( wnd, &ps );
      SelectFont( dc, GetStockFont( SYSTEM_FIXED_FONT ) );
      SetTextColor( dc, GetSysColor( COLOR_BTNTEXT ) );
      SetBkColor( dc, GetSysColor( COLOR_BTNFACE ) );

      // assume the file is ASCII
      DrawTextA( dc, buffer, bytesRead, &rect, DTFLAGS );

      EndPaint( wnd, &ps );
      return 0;

   case WM_DESTROY:
      PostQuitMessage( 0 );
      return 0;
   }

   return DefWindowProcW( wnd, msg, wParam, lParam );
}

LRESULT CALLBACK ListProc( HWND wnd,
                           UINT msg,
                           WPARAM wParam,
                           LPARAM lParam,
                           UINT_PTR IDSubclass,
                           DWORD_PTR refData )
{
   UNREFERENCED_PARAMETER( IDSubclass );
   UNREFERENCED_PARAMETER( refData );

   switch ( msg )
   {
   case WM_KEYDOWN:
      if ( wParam == VK_RETURN )
      {
         SendMessageW( GetParent( wnd ), WM_COMMAND,
                       MAKELONG( ID_LIST, LBN_DBLCLK ), ( LPARAM ) wnd );
      }
      break;

   case WM_NCDESTROY:
      RemoveWindowSubclass( wnd, ListProc, ID_LIST );
      break;

   default:
      break;
   }
   return DefSubclassProc( wnd, msg, wParam, lParam );
}
