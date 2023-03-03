/*---------------------------------------------
   OWNDRAW.C -- Owner-Draw Button Demo Program
            (c) Charles Petzold, 1998
  ---------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>

#define ID_SMALLER 1
#define ID_LARGER  2

#define BTN_WIDTH  (8 * cxChar)
#define BTN_HEIGHT (4 * cyChar)

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE inst;

int WINAPI wWinMain(_In_     HINSTANCE instance,
                    _In_opt_ HINSTANCE prevInstance,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInstance);
   UNREFERENCED_PARAMETER(cmdLine);

   static PCWSTR  appName = L"OwnDraw";
   MSG            msg;
   HWND           hwnd;
   WNDCLASSW      wc;

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
      MessageBoxW(NULL, L"This program requires Windows NT!",
                  appName, MB_ICONERROR);
      return 0;
   }

   hwnd = CreateWindowW(appName, L"Owner-Draw Button Demo",
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

void Triangle(HDC hdc, POINT pt[])
{
   SelectBrush(hdc, GetStockBrush(BLACK_BRUSH));
   Polygon(hdc, pt, 3);
   SelectBrush(hdc, GetStockBrush(WHITE_BRUSH));
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   static HWND      smaller;
   static HWND      larger;
   static int       cxClient;
   static int       cyClient;
   static int       cxChar;
   static int       cyChar;
   int              cx;
   int              cy;
   LPDRAWITEMSTRUCT pdis;
   POINT            pt[ 3 ];
   RECT             rc;

   switch ( message )
   {
   case WM_CREATE:
      cxChar = LOWORD(GetDialogBaseUnits());
      cyChar = HIWORD(GetDialogBaseUnits());

      // create the owner-draw pushbuttons
      smaller = CreateWindowW(L"button", L"",
                              WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                              0, 0, BTN_WIDTH, BTN_HEIGHT,
                              hwnd, (HMENU) ID_SMALLER, inst, NULL);

      larger = CreateWindowW(L"button", L"",
                             WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                             0, 0, BTN_WIDTH, BTN_HEIGHT,
                             hwnd, (HMENU) ID_LARGER, inst, NULL);
      return 0;

   case WM_SIZE:
      cxClient = GET_X_LPARAM(lParam);
      cyClient = GET_Y_LPARAM(lParam);

      // move the buttons to the new center
      MoveWindow(smaller, cxClient / 2 - 3 * BTN_WIDTH / 2,
                          cyClient / 2 - BTN_HEIGHT / 2,
                          BTN_WIDTH, BTN_HEIGHT, TRUE);

      MoveWindow(larger, cxClient / 2 + BTN_WIDTH / 2,
                         cyClient / 2 - BTN_HEIGHT / 2,
                         BTN_WIDTH, BTN_HEIGHT, TRUE);
      return 0;

   case WM_COMMAND:
      GetWindowRect(hwnd, &rc);

      // make the window 10% smaller or larger
      switch ( wParam )
      {
      case ID_SMALLER:
         rc.left   += cxClient / 20;
         rc.right  -= cxClient / 20;
         rc.top    += cyClient / 20;
         rc.bottom -= cyClient / 20;
         break;

      case ID_LARGER:
         rc.left   -= cxClient / 20;
         rc.right  += cxClient / 20;
         rc.top    -= cyClient / 20;
         rc.bottom += cyClient / 20;
         break;
      }

      MoveWindow(hwnd, rc.left, rc.top, rc.right - rc.left,
                 rc.bottom - rc.top, TRUE);
      return 0;

   case WM_DRAWITEM:
      pdis = (LPDRAWITEMSTRUCT) lParam;

      // fill area with white and frame it black
      FillRect(pdis->hDC, &pdis->rcItem, GetStockBrush(WHITE_BRUSH));

      FrameRect(pdis->hDC, &pdis->rcItem, GetStockBrush(BLACK_BRUSH));

      // draw inward and outward black triangles
      cx = pdis->rcItem.right - pdis->rcItem.left;
      cy = pdis->rcItem.bottom - pdis->rcItem.top;

      switch ( pdis->CtlID )
      {
      case ID_SMALLER:
         pt[ 0 ].x = 3 * cx / 8;  pt[ 0 ].y = 1 * cy / 8;
         pt[ 1 ].x = 5 * cx / 8;  pt[ 1 ].y = 1 * cy / 8;
         pt[ 2 ].x = 4 * cx / 8;  pt[ 2 ].y = 3 * cy / 8;

         Triangle(pdis->hDC, pt);

         pt[ 0 ].x = 7 * cx / 8;  pt[ 0 ].y = 3 * cy / 8;
         pt[ 1 ].x = 7 * cx / 8;  pt[ 1 ].y = 5 * cy / 8;
         pt[ 2 ].x = 5 * cx / 8;  pt[ 2 ].y = 4 * cy / 8;

         Triangle(pdis->hDC, pt);

         pt[ 0 ].x = 5 * cx / 8;  pt[ 0 ].y = 7 * cy / 8;
         pt[ 1 ].x = 3 * cx / 8;  pt[ 1 ].y = 7 * cy / 8;
         pt[ 2 ].x = 4 * cx / 8;  pt[ 2 ].y = 5 * cy / 8;

         Triangle(pdis->hDC, pt);

         pt[ 0 ].x = 1 * cx / 8;  pt[ 0 ].y = 5 * cy / 8;
         pt[ 1 ].x = 1 * cx / 8;  pt[ 1 ].y = 3 * cy / 8;
         pt[ 2 ].x = 3 * cx / 8;  pt[ 2 ].y = 4 * cy / 8;

         Triangle(pdis->hDC, pt);
         break;

      case ID_LARGER:
         pt[ 0 ].x = 5 * cx / 8;  pt[ 0 ].y = 3 * cy / 8;
         pt[ 1 ].x = 3 * cx / 8;  pt[ 1 ].y = 3 * cy / 8;
         pt[ 2 ].x = 4 * cx / 8;  pt[ 2 ].y = 1 * cy / 8;

         Triangle(pdis->hDC, pt);

         pt[ 0 ].x = 5 * cx / 8;  pt[ 0 ].y = 5 * cy / 8;
         pt[ 1 ].x = 5 * cx / 8;  pt[ 1 ].y = 3 * cy / 8;
         pt[ 2 ].x = 7 * cx / 8;  pt[ 2 ].y = 4 * cy / 8;

         Triangle(pdis->hDC, pt);

         pt[ 0 ].x = 3 * cx / 8;  pt[ 0 ].y = 5 * cy / 8;
         pt[ 1 ].x = 5 * cx / 8;  pt[ 1 ].y = 5 * cy / 8;
         pt[ 2 ].x = 4 * cx / 8;  pt[ 2 ].y = 7 * cy / 8;

         Triangle(pdis->hDC, pt);

         pt[ 0 ].x = 3 * cx / 8;  pt[ 0 ].y = 3 * cy / 8;
         pt[ 1 ].x = 3 * cx / 8;  pt[ 1 ].y = 5 * cy / 8;
         pt[ 2 ].x = 1 * cx / 8;  pt[ 2 ].y = 4 * cy / 8;

         Triangle(pdis->hDC, pt);
         break;
      }

      // invert the rectangle if the button is selected
      if ( pdis->itemState & ODS_SELECTED )
         InvertRect(pdis->hDC, &pdis->rcItem);

      // draw a focus rectangle if the button has the focus
      if ( pdis->itemState & ODS_FOCUS )
      {
         pdis->rcItem.left   += cx / 16;
         pdis->rcItem.top    += cy / 16;
         pdis->rcItem.right  -= cx / 16;
         pdis->rcItem.bottom -= cy / 16;

         DrawFocusRect(pdis->hDC, &pdis->rcItem);
      }
      return 0;

   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(hwnd, message, wParam, lParam);
}