/*----------------------------------------
   BTNLOOK.C -- Button Look Program
                (c) Charles Petzold, 1998
  ----------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>

struct
{
   int    style;
   PCWSTR text;
}
button[ ] = { {BS_PUSHBUTTON,      L"PUSHBUTTON"},
              {BS_DEFPUSHBUTTON,   L"DEFPUSHBUTTON"},
              {BS_CHECKBOX,        L"CHECKBOX"},
              {BS_AUTOCHECKBOX,    L"AUTOCHECKBOX"},
              {BS_RADIOBUTTON,     L"RADIOBUTTON"},
              {BS_3STATE,          L"3STATE"},
              {BS_AUTO3STATE,      L"AUTO3STATE"},
              {BS_GROUPBOX,        L"GROUPBOX"},
              {BS_AUTORADIOBUTTON, L"AUTORADIO"},
              {BS_OWNERDRAW,       L"OWNERDRAW"} };

#define NUM _countof(button)

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(_In_     HINSTANCE inst,
                    _In_opt_ HINSTANCE prevInst,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInst);
   UNREFERENCED_PARAMETER(cmdLine);

   static PCWSTR appName = L"BtnLook";
   HWND          wnd;
   MSG           msg;
   WNDCLASSW     wc      = { 0 };

   wc.style         = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = inst;
   wc.hIcon         = (HICON)   LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wc.hbrBackground = (HBRUSH)  (COLOR_WINDOW + 1);
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = appName;

   if ( !RegisterClassW(&wc) )
   {
      MessageBoxW(NULL, L"This program requires Windows NT!", appName, MB_ICONERROR);
      return 0;
   }

   wnd = CreateWindowW(appName, L"Button Look",
                       WS_OVERLAPPEDWINDOW,
                       CW_USEDEFAULT, CW_USEDEFAULT,
                       CW_USEDEFAULT, CW_USEDEFAULT,
                       NULL, NULL, inst, NULL);

   ShowWindow(wnd, showCmd);
   UpdateWindow(wnd);

   while ( GetMessageW(&msg, NULL, 0, 0) )
   {
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
   }
   return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static HWND   wndButton[ NUM ];
   static RECT   rect;

   static PCWSTR top    = L"message wParam lParam";
   static PCWSTR und    = L"_______ ______ ______";
   static PCWSTR format = L"%-16s%04X-%04X %04X-%04X";

   static WCHAR  buffer[ 50 ];
   static int    xChar;
   static int    yChar;
   HDC           dc;
   PAINTSTRUCT   ps;

   switch ( msg )
   {
   case WM_CREATE:
      xChar = LOWORD(GetDialogBaseUnits( ));
      yChar = HIWORD(GetDialogBaseUnits( ));

      for ( unsigned i = 0; i < NUM; i++ )
      {
         wndButton[ i ] = CreateWindowExW(0, L"button",
                                          button[ i ].text,
                                          WS_CHILD | WS_VISIBLE | button[ i ].style,
                                          xChar, yChar * (1 + 2 * i),
                                          20 * xChar, 7 * yChar / 4,
                                          wnd, (HMENU) i,
                                          ((LPCREATESTRUCT) lParam)->hInstance, NULL);
      }
      return 0;

   case WM_SIZE:
      rect.left   = 24 * xChar;
      rect.top    = 2 * yChar;
      rect.right  = GET_X_LPARAM(lParam);
      rect.bottom = GET_Y_LPARAM(lParam);
      return 0;

   case WM_PAINT:
      InvalidateRect(wnd, &rect, TRUE);

      dc = BeginPaint(wnd, &ps);
      SelectFont(dc, GetStockFont(SYSTEM_FIXED_FONT));
      SetBkMode(dc, TRANSPARENT);

      TextOutW(dc, 24 * xChar, yChar, top, lstrlenW(top));
      TextOutW(dc, 24 * xChar, yChar, und, lstrlenW(und));

      EndPaint(wnd, &ps);
      return 0;

   case WM_DRAWITEM:
   case WM_COMMAND:
      ScrollWindow(wnd, 0, -yChar, &rect, &rect);

      dc = GetDC(wnd);
      SelectFont(dc, GetStockFont(SYSTEM_FIXED_FONT));

      TextOutW(dc, 24 * xChar, yChar * (rect.bottom / yChar - 1),
               buffer,
               wsprintfW(buffer, format,
                         msg == WM_DRAWITEM ? L"WM_DRAWITEM" :
                         L"WM_COMMAND",
                         HIWORD(wParam), LOWORD(wParam),
                         HIWORD(lParam), LOWORD(lParam)));

      ReleaseDC(wnd, dc);
      ValidateRect(wnd, &rect);
      break;

   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(wnd, msg, wParam, lParam);
}