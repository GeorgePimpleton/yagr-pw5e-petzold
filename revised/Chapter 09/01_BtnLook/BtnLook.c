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
button[] = { {BS_PUSHBUTTON,      L"PUSHBUTTON"},
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

int WINAPI wWinMain(_In_     HINSTANCE instance,
                    _In_opt_ HINSTANCE prevInstance,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInstance);
   UNREFERENCED_PARAMETER(cmdLine);

   static PCWSTR  appName = L"BtnLook";
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

   hwnd = CreateWindowW(appName, L"Button Look",
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        NULL, NULL, instance, NULL);

   ShowWindow(hwnd, showCmd);
   UpdateWindow(hwnd);

   while ( GetMessageW(&msg, NULL, 0, 0) )
   {
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
   }
   return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   static HWND   hwndButton[ NUM ];
   static RECT   rect;
   static PCWSTR top    = L"message            wParam       lParam";
   static PCWSTR und    = L"_______            ______       ______";
   static PCWSTR format = L"%-16s%04X-%04X    %04X-%04X";
   static WCHAR  buffer[ 50 ];
   static int    cxChar;
   static int    cyChar;
   HDC           hdc;
   PAINTSTRUCT   ps;

   switch ( message )
   {
   case WM_CREATE:
      cxChar = LOWORD(GetDialogBaseUnits());
      cyChar = HIWORD(GetDialogBaseUnits());

      for ( unsigned i = 0; i < NUM; i++ )
         hwndButton[ i ] = CreateWindowExW(0, L"button",
                                           button[ i ].text,
                                           WS_CHILD | WS_VISIBLE | button[ i ].style,
                                           cxChar, cyChar * (1 + 2 * i),
                                           20 * cxChar, 7 * cyChar / 4,
                                           hwnd, (HMENU) i,
                                           ((LPCREATESTRUCT) lParam)->hInstance, NULL);
      return 0;

   case WM_SIZE:
      rect.left   = 24 * cxChar;
      rect.top    = 2 * cyChar;
      rect.right  = GET_X_LPARAM(lParam);
      rect.bottom = GET_Y_LPARAM(lParam);
      return 0;

   case WM_PAINT:
      InvalidateRect(hwnd, &rect, TRUE);

      hdc = BeginPaint(hwnd, &ps);
      SelectFont(hdc, GetStockFont(SYSTEM_FIXED_FONT));
      SetBkMode(hdc, TRANSPARENT);

      TextOutW(hdc, 24 * cxChar, cyChar, top, lstrlenW(top));
      TextOutW(hdc, 24 * cxChar, cyChar, und, lstrlenW(und));

      EndPaint(hwnd, &ps);
      return 0;

   case WM_DRAWITEM:
   case WM_COMMAND:
      ScrollWindow(hwnd, 0, -cyChar, &rect, &rect);

      hdc = GetDC(hwnd);
      SelectFont(hdc, GetStockFont(SYSTEM_FIXED_FONT));

      TextOutW(hdc, 24 * cxChar, cyChar * (rect.bottom / cyChar - 1),
               buffer,
               wsprintfW(buffer, format,
                         message == WM_DRAWITEM ? L"WM_DRAWITEM" :
                         L"WM_COMMAND",
                         HIWORD(wParam), LOWORD(wParam),
                         HIWORD(lParam), LOWORD(lParam)));

      ReleaseDC(hwnd, hdc);
      ValidateRect(hwnd, &rect);
      break;

   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(hwnd, message, wParam, lParam);
}