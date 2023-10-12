/*---------------------------------------------
   OWNDRAW.C -- Owner-Draw Button Demo Program
                (c) Charles Petzold, 1998
  ---------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>

#define ID_SMALLER 1
#define ID_LARGER  2

#define BTN_WIDTH  (8 * xChar)
#define BTN_HEIGHT (4 * yChar)

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE inst;

int WINAPI wWinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE prevInstance,
                    _In_ PWSTR     cmdLine,  _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInstance);
   UNREFERENCED_PARAMETER(cmdLine);

   static PCWSTR appName = L"OwnDraw";
   MSG           msg;
   HWND          wnd;
   WNDCLASSW     wc;

   inst = instance;

   wc.style         = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = instance;
   wc.hIcon         = (HICON)   LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wc.hbrBackground = (HBRUSH)  GetStockBrush(WHITE_BRUSH);
   wc.lpszMenuName  = appName;
   wc.lpszClassName = appName;

   if ( !RegisterClassW(&wc) )
   {
      MessageBoxW(NULL, L"This program requires Windows NT!", appName, MB_ICONERROR);
      return 0;
   }

   wnd = CreateWindowW(appName, L"Owner-Draw Button Demo",
                       WS_OVERLAPPEDWINDOW,
                       CW_USEDEFAULT, CW_USEDEFAULT,
                       CW_USEDEFAULT, CW_USEDEFAULT,
                       NULL, NULL, instance, NULL);

   ShowWindow(wnd, showCmd);
   UpdateWindow(wnd);

   while ( GetMessageW(&msg, NULL, 0, 0) )
   {
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
   }
   return (int) msg.wParam;
}

void Triangle(HDC dc, POINT pt[ ])
{
   SelectBrush(dc, GetStockBrush(BLACK_BRUSH));
   Polygon(dc, pt, 3);
   SelectBrush(dc, GetStockBrush(WHITE_BRUSH));
}

LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static HWND      smaller;
   static HWND      larger;
   static int       xClient;
   static int       yClient;
   static int       xChar;
   static int       yChar;
   int              x;
   int              y;
   LPDRAWITEMSTRUCT is;
   POINT            pt[ 3 ];
   RECT             rc;

   switch ( msg )
   {
   case WM_CREATE:
      xChar = LOWORD(GetDialogBaseUnits( ));
      yChar = HIWORD(GetDialogBaseUnits( ));

      // create the owner-draw pushbuttons
      smaller = CreateWindowW(L"button", L"",
                              WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                              0, 0, BTN_WIDTH, BTN_HEIGHT,
                              wnd, (HMENU) ID_SMALLER, inst, NULL);

      larger = CreateWindowW(L"button", L"",
                             WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                             0, 0, BTN_WIDTH, BTN_HEIGHT,
                             wnd, (HMENU) ID_LARGER, inst, NULL);
      return 0;

   case WM_SIZE:
      xClient = GET_X_LPARAM(lParam);
      yClient = GET_Y_LPARAM(lParam);

      // move the buttons to the new center
      MoveWindow(smaller, xClient / 2 - 3 * BTN_WIDTH / 2,
                          yClient / 2 - BTN_HEIGHT / 2,
                          BTN_WIDTH, BTN_HEIGHT, TRUE);

      MoveWindow(larger, xClient / 2 + BTN_WIDTH / 2,
                         yClient / 2 - BTN_HEIGHT / 2,
                         BTN_WIDTH, BTN_HEIGHT, TRUE);
      return 0;

   case WM_COMMAND:
      GetWindowRect(wnd, &rc);

      // make the window 10% smaller or larger
      switch ( wParam )
      {
      case ID_SMALLER:
         rc.left   += xClient / 20;
         rc.right  -= xClient / 20;
         rc.top    += yClient / 20;
         rc.bottom -= yClient / 20;
         break;

      case ID_LARGER:
         rc.left   -= xClient / 20;
         rc.right  += xClient / 20;
         rc.top    -= yClient / 20;
         rc.bottom += yClient / 20;
         break;
      }

      MoveWindow(wnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
      return 0;

   case WM_DRAWITEM:
      is = (LPDRAWITEMSTRUCT) lParam;

      // fill area with white and frame it black
      FillRect(is->hDC, &is->rcItem, GetStockBrush(WHITE_BRUSH));

      FrameRect(is->hDC, &is->rcItem, GetStockBrush(BLACK_BRUSH));

      // draw inward and outward black triangles
      x = is->rcItem.right - is->rcItem.left;
      y = is->rcItem.bottom - is->rcItem.top;

      switch ( is->CtlID )
      {
      case ID_SMALLER:
         pt[ 0 ].x = 3 * x / 8;  pt[ 0 ].y = 1 * y / 8;
         pt[ 1 ].x = 5 * x / 8;  pt[ 1 ].y = 1 * y / 8;
         pt[ 2 ].x = 4 * x / 8;  pt[ 2 ].y = 3 * y / 8;

         Triangle(is->hDC, pt);

         pt[ 0 ].x = 7 * x / 8;  pt[ 0 ].y = 3 * y / 8;
         pt[ 1 ].x = 7 * x / 8;  pt[ 1 ].y = 5 * y / 8;
         pt[ 2 ].x = 5 * x / 8;  pt[ 2 ].y = 4 * y / 8;

         Triangle(is->hDC, pt);

         pt[ 0 ].x = 5 * x / 8;  pt[ 0 ].y = 7 * y / 8;
         pt[ 1 ].x = 3 * x / 8;  pt[ 1 ].y = 7 * y / 8;
         pt[ 2 ].x = 4 * x / 8;  pt[ 2 ].y = 5 * y / 8;

         Triangle(is->hDC, pt);

         pt[ 0 ].x = 1 * x / 8;  pt[ 0 ].y = 5 * y / 8;
         pt[ 1 ].x = 1 * x / 8;  pt[ 1 ].y = 3 * y / 8;
         pt[ 2 ].x = 3 * x / 8;  pt[ 2 ].y = 4 * y / 8;

         Triangle(is->hDC, pt);
         break;

      case ID_LARGER:
         pt[ 0 ].x = 5 * x / 8;  pt[ 0 ].y = 3 * y / 8;
         pt[ 1 ].x = 3 * x / 8;  pt[ 1 ].y = 3 * y / 8;
         pt[ 2 ].x = 4 * x / 8;  pt[ 2 ].y = 1 * y / 8;

         Triangle(is->hDC, pt);

         pt[ 0 ].x = 5 * x / 8;  pt[ 0 ].y = 5 * y / 8;
         pt[ 1 ].x = 5 * x / 8;  pt[ 1 ].y = 3 * y / 8;
         pt[ 2 ].x = 7 * x / 8;  pt[ 2 ].y = 4 * y / 8;

         Triangle(is->hDC, pt);

         pt[ 0 ].x = 3 * x / 8;  pt[ 0 ].y = 5 * y / 8;
         pt[ 1 ].x = 5 * x / 8;  pt[ 1 ].y = 5 * y / 8;
         pt[ 2 ].x = 4 * x / 8;  pt[ 2 ].y = 7 * y / 8;

         Triangle(is->hDC, pt);

         pt[ 0 ].x = 3 * x / 8;  pt[ 0 ].y = 3 * y / 8;
         pt[ 1 ].x = 3 * x / 8;  pt[ 1 ].y = 5 * y / 8;
         pt[ 2 ].x = 1 * x / 8;  pt[ 2 ].y = 4 * y / 8;

         Triangle(is->hDC, pt);
         break;
      }

      // invert the rectangle if the button is selected
      if ( is->itemState & ODS_SELECTED )
      {
         InvertRect(is->hDC, &is->rcItem);
      }

      // draw a focus rectangle if the button has the focus
      if ( is->itemState & ODS_FOCUS )
      {
         is->rcItem.left += x / 16;
         is->rcItem.top += y / 16;
         is->rcItem.right -= x / 16;
         is->rcItem.bottom -= y / 16;

         DrawFocusRect(is->hDC, &is->rcItem);
      }
      return 0;

   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(wnd, msg, wParam, lParam);
}