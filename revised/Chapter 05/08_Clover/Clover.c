/*--------------------------------------------------
   CLOVER.C -- Clover Drawing Program Using Regions
            (c) Charles Petzold, 1998
  --------------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>

#define _USE_MATH_DEFINES
#include <math.h>

#define TWO_PI (2.0 * M_PI)

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(_In_     HINSTANCE hInstance,
                    _In_opt_ HINSTANCE hPrevInstance,
                    _In_     PWSTR     pCmdLine,
                    _In_     int       nShowCmd)
{
   UNREFERENCED_PARAMETER(hPrevInstance);
   UNREFERENCED_PARAMETER(pCmdLine);

   static PCWSTR  szAppName = L"Clover";
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
   wc.hbrBackground = (HBRUSH)  (COLOR_WINDOW + 1);
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = szAppName;

   if ( !RegisterClassW(&wc) )
   {
      MessageBoxW(NULL, L"This program requires Windows NT!",
                  szAppName, MB_ICONERROR);
      return 0;
   }

   hwnd = CreateWindowW(szAppName, L"Draw a Clover",
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

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
   static HRGN hRgnClip;
   static int  cxClient;
   static int  cyClient;
   double      fAngle;
   double      fRadius;
   HCURSOR     hCursor;
   HDC         hdc;
   HRGN        hRgnTemp[ 6 ];
   int         i;
   PAINTSTRUCT ps;

   switch ( iMsg )
   {
   case WM_SIZE:
      cxClient = GET_X_LPARAM(lParam);
      cyClient = GET_Y_LPARAM(lParam);

      hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
      ShowCursor(TRUE);

      if ( hRgnClip )
         DeleteObject(hRgnClip);

      hRgnTemp[ 0 ] = CreateEllipticRgn(0, cyClient / 3, cxClient / 2, 2 * cyClient / 3);
      hRgnTemp[ 1 ] = CreateEllipticRgn(cxClient / 2, cyClient / 3, cxClient, 2 * cyClient / 3);
      hRgnTemp[ 2 ] = CreateEllipticRgn(cxClient / 3, 0, 2 * cxClient / 3, cyClient / 2);
      hRgnTemp[ 3 ] = CreateEllipticRgn(cxClient / 3, cyClient / 2, 2 * cxClient / 3, cyClient);
      hRgnTemp[ 4 ] = CreateRectRgn(0, 0, 1, 1);
      hRgnTemp[ 5 ] = CreateRectRgn(0, 0, 1, 1);
      hRgnClip      = CreateRectRgn(0, 0, 1, 1);

      CombineRgn(hRgnTemp[ 4 ], hRgnTemp[ 0 ], hRgnTemp[ 1 ], RGN_OR);
      CombineRgn(hRgnTemp[ 5 ], hRgnTemp[ 2 ], hRgnTemp[ 3 ], RGN_OR);
      CombineRgn(hRgnClip, hRgnTemp[ 4 ], hRgnTemp[ 5 ], RGN_XOR);

      for ( i = 0; i < 6; i++ )
         DeleteObject(hRgnTemp[ i ]);

      SetCursor(hCursor);
      ShowCursor(FALSE);
      return 0;

   case WM_PAINT: hdc = BeginPaint(hwnd, &ps);

      SetViewportOrgEx(hdc, cxClient / 2, cyClient / 2, NULL);
      SelectClipRgn(hdc, hRgnClip);

      fRadius = _hypot(cxClient / 2.0, cyClient / 2.0);

      for ( fAngle = 0.0; fAngle < TWO_PI; fAngle += TWO_PI / 360 )
      {
         MoveToEx(hdc, 0, 0, NULL);
         LineTo(hdc, (int) (fRadius * cos(fAngle) + 0.5),
                     (int) (-fRadius * sin(fAngle) + 0.5));
      }
      EndPaint(hwnd, &ps);
      return 0;

   case WM_DESTROY: DeleteObject(hRgnClip);
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(hwnd, iMsg, wParam, lParam);
}