/*--------------------------------------
   TYPER.C -- Typing Program
           (c) Charles Petzold, 1998
  --------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <malloc.h>

#define BUFFER(x,y) *(buffer + y * xBuffer + x)

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(_In_     HINSTANCE instance,
                    _In_opt_ HINSTANCE prevInstance,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInstance);
   UNREFERENCED_PARAMETER(cmdLine);

   static PCWSTR  appName = L"Typer";
   HWND           hwnd;
   MSG            msg;
   WNDCLASSW      wc;

   wc.style         = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = instance;
   wc.hIcon         = (HICON)   LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wc.hbrBackground = (HBRUSH)  (COLOR_WINDOW + 1);
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = appName;

   if ( !RegisterClassW(&wc) )
   {
      MessageBoxW(NULL, L"This program requires Windows NT!",
                  appName, MB_ICONERROR);
      return 0;
   }

   hwnd = CreateWindowW(appName, L"Typing Program",
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        NULL, NULL, instance, NULL);

   ShowWindow(hwnd, showCmd);
   UpdateWindow(hwnd);

   while ( GetMessage(&msg, NULL, 0, 0) )
   {
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
   }
   return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   static DWORD charSet = DEFAULT_CHARSET;
   static int   xChar;
   static int   yChar;
   static int   xClient;
   static int   yClient;
   static int   xBuffer;
   static int   yBuffer;
   static int   xCaret;
   static int   yCaret;
   static PWSTR buffer   = NULL;
   HDC          dc;
   int          x;
   int          y;
   int          i;
   PAINTSTRUCT  ps;
   TEXTMETRICW  tm;

   switch ( message )
   {
   case WM_INPUTLANGCHANGE:
      charSet = (DWORD) wParam;
      // fall through

   case WM_CREATE:
      dc = GetDC(hwnd);
      SelectObject(dc, CreateFontW(0, 0, 0, 0, 0, 0, 0, 0,
                                    charSet, 0, 0, 0, FIXED_PITCH, NULL));

      GetTextMetricsW(dc, &tm);
      xChar = tm.tmAveCharWidth;
      yChar = tm.tmHeight;

      DeleteObject(SelectObject(dc, GetStockObject(SYSTEM_FONT)));
      ReleaseDC(hwnd, dc);
      // fall through

   case WM_SIZE:
      // obtain window size in pixels
      if ( message == WM_SIZE )
      {
         xClient = GET_X_LPARAM(lParam);
         yClient = GET_Y_LPARAM(lParam);
      }

      // calculate window size in characters
      xBuffer = max(1, xClient / xChar);
      yBuffer = max(1, yClient / yChar);

      // allocate memory for buffer and clear it
      if ( buffer != NULL )
      {
         free(buffer);
      }

      buffer = (PWSTR) malloc(xBuffer * yBuffer * sizeof(WCHAR));

      for ( y = 0; y < yBuffer; y++ )
      {
         for ( x = 0; x < xBuffer; x++ )
         {
            BUFFER(x, y) = ' ';
         }
      }

      // set caret to upper left corner

      xCaret = 0;
      yCaret = 0;

      if ( hwnd == GetFocus( ) )
      {
         SetCaretPos(xCaret * xChar, yCaret * yChar);
      }

      InvalidateRect(hwnd, NULL, TRUE);
      return 0;

   case WM_SETFOCUS:
      // create and show the caret
      CreateCaret(hwnd, NULL, xChar, yChar);
      SetCaretPos(xCaret * xChar, yCaret * yChar);
      ShowCaret(hwnd);
      return 0;

   case WM_KILLFOCUS:
      // hide and destroy the caret
      HideCaret(hwnd);
      DestroyCaret();
      return 0;

   case WM_KEYDOWN:
      switch ( wParam )
      {
      case VK_HOME:
         xCaret = 0;
         break;

      case VK_END:
         xCaret = xBuffer - 1;
         break;

      case VK_PRIOR:
         yCaret = 0;
         break;

      case VK_NEXT:
         yCaret = yBuffer - 1;
         break;

      case VK_LEFT:
         xCaret = max(xCaret - 1, 0);
         break;

      case VK_RIGHT:
         xCaret = min(xCaret + 1, xBuffer - 1);
         break;

      case VK_UP:
         yCaret = max(yCaret - 1, 0);
         break;

      case VK_DOWN:
         yCaret = min(yCaret + 1, yBuffer - 1);
         break;

      case VK_DELETE:
         for ( x = xCaret; x < xBuffer - 1; x++ )
         {
            BUFFER(x, yCaret) = BUFFER(x + 1, yCaret);
         }

         BUFFER(xBuffer - 1, yCaret) = ' ';

         HideCaret(hwnd);
         dc = GetDC(hwnd);

         SelectObject(dc, CreateFontW(0, 0, 0, 0, 0, 0, 0, 0,
                                      charSet, 0, 0, 0, FIXED_PITCH, NULL));

         TextOutW(dc, xCaret * xChar, yCaret * yChar,
                  &BUFFER(xCaret, yCaret),
                  xBuffer - xCaret);

         DeleteObject(SelectObject(dc, GetStockObject(SYSTEM_FONT)));
         ReleaseDC(hwnd, dc);
         ShowCaret(hwnd);
         break;
      }
      SetCaretPos(xCaret * xChar, yCaret * yChar);
      return 0;

   case WM_CHAR:
      for ( i = 0; i < (int) LOWORD(lParam); i++ )
      {
         switch ( wParam )
         {
         case '\b':  // backspace
            if ( xCaret > 0 )
            {
               xCaret--;
               SendMessageW(hwnd, WM_KEYDOWN, VK_DELETE, 1);
            }
            break;

         case '\t':  // tab
            do
            {
               SendMessageW(hwnd, WM_CHAR, ' ', 1);
            } while ( xCaret % 8 != 0 );
            break;

         case '\n':  // line feed
            if ( ++yCaret == yBuffer )
            {
               yCaret = 0;
            }
            break;

         case '\r':  // carriage return
            xCaret = 0;

            if ( ++yCaret == yBuffer )
            {
               yCaret = 0;
            }
            break;

         case '\x1B':   // escape
            for ( y = 0; y < yBuffer; y++ )
            {
               for ( x = 0; x < xBuffer; x++ )
               {
                  BUFFER(x, y) = ' ';
               }
            }

            xCaret = 0;
            yCaret = 0;

            InvalidateRect(hwnd, NULL, FALSE);
            break;

         default:                      // character codes
            BUFFER(xCaret, yCaret) = (WCHAR) wParam;

            HideCaret(hwnd);
            dc = GetDC(hwnd);

            SelectObject(dc, CreateFontW(0, 0, 0, 0, 0, 0, 0, 0,
                                          charSet, 0, 0, 0, FIXED_PITCH, NULL));

            TextOutW(dc, xCaret* xChar, yCaret* yChar,
                     &BUFFER(xCaret, yCaret), 1);

            DeleteObject(
               SelectObject(dc, GetStockObject(SYSTEM_FONT)));
            ReleaseDC(hwnd, dc);
            ShowCaret(hwnd);

            if ( ++xCaret == xBuffer )
            {
               xCaret = 0;

               if ( ++yCaret == yBuffer )
               {
                  yCaret = 0;
               }
            }

            break;
         }
      }

      SetCaretPos(xCaret * xChar, yCaret * yChar);
      return 0;

   case WM_PAINT:
      dc = BeginPaint(hwnd, &ps);

      SelectObject(dc, CreateFontW(0, 0, 0, 0, 0, 0, 0, 0,
                                   charSet, 0, 0, 0, FIXED_PITCH, NULL));

      for ( y = 0; y < yBuffer; y++ )
      {
         TextOutW(dc, 0, y * yChar, &BUFFER(0, y), xBuffer);
      }

      DeleteObject(SelectObject(dc, GetStockObject(SYSTEM_FONT)));
      EndPaint(hwnd, &ps);
      return 0;

   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(hwnd, message, wParam, lParam);
}