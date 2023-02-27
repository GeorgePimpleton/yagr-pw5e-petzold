/*-----------------------------------------------
   ALTWIND.C -- Alternate and Winding Fill Modes
            (c) Charles Petzold, 1998
  -----------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(_In_     HINSTANCE hInstance,
                    _In_opt_ HINSTANCE hPrevInstance,
                    _In_     PWSTR     pCmdLine,
                    _In_     int       nShowCmd)
{
   UNREFERENCED_PARAMETER(hPrevInstance);
   UNREFERENCED_PARAMETER(pCmdLine);

   static PCWSTR  szAppName = L"AltWind";
   HWND           hwnd;
   MSG            msg;
   WNDCLASSW      wc;

   wc.style         = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = hInstance;
   wc.hIcon         = (HICON)   LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wc.hbrBackground = (HBRUSH)  GetStockObject(WHITE_BRUSH);
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = szAppName;

   if ( !RegisterClassW(&wc) )
   {
      MessageBoxW(NULL, L"Program requires Windows NT!",
                  szAppName, MB_ICONERROR);
      return 0;
   }

   hwnd = CreateWindowW(szAppName, L"Alternate and Winding Fill Modes",
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        NULL, NULL, hInstance, NULL);

   ShowWindow(hwnd, nShowCmd);
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
   static POINT aptFigure[ 10 ] = { {10,70}, {50,70}, {50,10}, {90,10}, {90,50},
                                    {30,50}, {30,90}, {70,90}, {70,30}, {10,30} };
   static int   cxClient;
   static int   cyClient;
   HDC          hdc;
   int          i;
   PAINTSTRUCT  ps;
   POINT        apt[ 10 ];

   switch ( message )
   {
   case WM_SIZE:
      cxClient = GET_X_LPARAM(lParam);
      cyClient = GET_Y_LPARAM(lParam);
      return 0;

   case WM_PAINT:
      hdc = BeginPaint(hwnd, &ps);

      SelectObject(hdc, GetStockObject(LTGRAY_BRUSH));

      for ( i = 0; i < 10; i++ )
      {
         apt[ i ].x = cxClient * aptFigure[ i ].x / 200;
         apt[ i ].y = cyClient * aptFigure[ i ].y / 100;
      }

      SetPolyFillMode(hdc, ALTERNATE);
      Polygon(hdc, apt, 10);

      for ( i = 0; i < 10; i++ )
      {
         apt[ i ].x += cxClient / 2;
      }

      SetPolyFillMode(hdc, WINDING);
      Polygon(hdc, apt, 10);

      EndPaint(hwnd, &ps);
      return 0;

   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   }
   return DefWindowProcW(hwnd, message, wParam, lParam);
}