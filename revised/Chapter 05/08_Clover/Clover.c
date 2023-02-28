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

int WINAPI wWinMain(_In_     HINSTANCE instance,
                    _In_opt_ HINSTANCE prevInstance,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInstance);
   UNREFERENCED_PARAMETER(cmdLine);

   static PCWSTR  appName = L"Clover";
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

   hwnd = CreateWindowW(appName, L"Draw a Clover",
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

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
   static HRGN rgnClip;
   static int  cxClient;
   static int  cyClient;
   double      angle;
   double      radius;
   HCURSOR     cursor;
   HDC         hdc;
   HRGN        rgnTemp[ 6 ];
   int         i;
   PAINTSTRUCT ps;

   switch ( iMsg )
   {
   case WM_SIZE:
      cxClient = GET_X_LPARAM(lParam);
      cyClient = GET_Y_LPARAM(lParam);

      cursor = SetCursor((HCURSOR) LoadImageW(NULL, IDC_WAIT, IMAGE_CURSOR, 0, 0, LR_SHARED));
      ShowCursor(TRUE);

      if ( rgnClip )
         DeleteObject(rgnClip);

      rgnTemp[ 0 ] = CreateEllipticRgn(0, cyClient / 3, cxClient / 2, 2 * cyClient / 3);
      rgnTemp[ 1 ] = CreateEllipticRgn(cxClient / 2, cyClient / 3, cxClient, 2 * cyClient / 3);
      rgnTemp[ 2 ] = CreateEllipticRgn(cxClient / 3, 0, 2 * cxClient / 3, cyClient / 2);
      rgnTemp[ 3 ] = CreateEllipticRgn(cxClient / 3, cyClient / 2, 2 * cxClient / 3, cyClient);
      rgnTemp[ 4 ] = CreateRectRgn(0, 0, 1, 1);
      rgnTemp[ 5 ] = CreateRectRgn(0, 0, 1, 1);
      rgnClip      = CreateRectRgn(0, 0, 1, 1);

      CombineRgn(rgnTemp[ 4 ], rgnTemp[ 0 ], rgnTemp[ 1 ], RGN_OR);
      CombineRgn(rgnTemp[ 5 ], rgnTemp[ 2 ], rgnTemp[ 3 ], RGN_OR);
      CombineRgn(rgnClip, rgnTemp[ 4 ], rgnTemp[ 5 ], RGN_XOR);

      for ( i = 0; i < 6; i++ )
         DeleteObject(rgnTemp[ i ]);

      SetCursor(cursor);
      ShowCursor(FALSE);
      return 0;

   case WM_PAINT: hdc = BeginPaint(hwnd, &ps);

      SetViewportOrgEx(hdc, cxClient / 2, cyClient / 2, NULL);
      SelectClipRgn(hdc, rgnClip);

      radius = _hypot(cxClient / 2.0, cyClient / 2.0);

      for ( angle = 0.0; angle < TWO_PI; angle += TWO_PI / 360 )
      {
         MoveToEx(hdc, 0, 0, NULL);
         LineTo(hdc, (int) (radius * cos(angle) + 0.5),
                     (int) (-radius * sin(angle) + 0.5));
      }
      EndPaint(hwnd, &ps);
      return 0;

   case WM_DESTROY: DeleteObject(rgnClip);
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(hwnd, iMsg, wParam, lParam);
}