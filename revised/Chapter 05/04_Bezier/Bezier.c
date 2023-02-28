/*---------------------------------------
   BEZIER.C -- Bezier Splines Demo
            (c) Charles Petzold, 1998
  ---------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void             DrawBezier(HDC, POINT[]);

int WINAPI wWinMain(_In_     HINSTANCE instance,
                    _In_opt_ HINSTANCE prevInstance,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInstance);
   UNREFERENCED_PARAMETER(cmdLine);

   static PCWSTR  appName = L"Bezier";
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
   wc.hbrBackground = (HBRUSH)  GetStockObject(WHITE_BRUSH);
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = appName;

   if ( !RegisterClassW(&wc) )
   {
      MessageBoxW(NULL, L"Program requires Windows NT!",
                  appName, MB_ICONERROR);
      return 0;
   }

   hwnd = CreateWindowW(appName, L"Bezier Splines",
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

void DrawBezier(HDC hdc, POINT apt[])
{
   PolyBezier(hdc, apt, 4);

   MoveToEx(hdc, apt[ 0 ].x, apt[ 0 ].y, NULL);
   LineTo(hdc, apt[ 1 ].x, apt[ 1 ].y);

   MoveToEx(hdc, apt[ 2 ].x, apt[ 2 ].y, NULL);
   LineTo(hdc, apt[ 3 ].x, apt[ 3 ].y);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   static POINT   apt[ 4 ];
   int            cxClient;
   int            cyClient;
   HDC            hdc;
   PAINTSTRUCT    ps;

   switch ( message )
   {
   case WM_SIZE:
      cxClient = GET_X_LPARAM(lParam);
      cyClient = GET_Y_LPARAM(lParam);

      apt[ 0 ].x = cxClient / 4;
      apt[ 0 ].y = cyClient / 2;

      apt[ 1 ].x = cxClient / 2;
      apt[ 1 ].y = cyClient / 4;

      apt[ 2 ].x = cxClient / 2;
      apt[ 2 ].y = 3 * cyClient / 4;

      apt[ 3 ].x = 3 * cxClient / 4;
      apt[ 3 ].y = cyClient / 2;

      return 0;

   case WM_LBUTTONDOWN:
   case WM_RBUTTONDOWN:
   case WM_MOUSEMOVE:
      if ( wParam & MK_LBUTTON || wParam & MK_RBUTTON )
      {
         hdc = GetDC(hwnd);

         SelectObject(hdc, GetStockObject(WHITE_PEN));
         DrawBezier(hdc, apt);

         if ( wParam & MK_LBUTTON )
         {
            apt[ 1 ].x = LOWORD(lParam);
            apt[ 1 ].y = HIWORD(lParam);
         }

         if ( wParam & MK_RBUTTON )
         {
            apt[ 2 ].x = LOWORD(lParam);
            apt[ 2 ].y = HIWORD(lParam);
         }

         SelectObject(hdc, GetStockObject(BLACK_PEN));
         DrawBezier(hdc, apt);
         ReleaseDC(hwnd, hdc);
      }
      return 0;

   case WM_PAINT:
      InvalidateRect(hwnd, NULL, TRUE);

      hdc = BeginPaint(hwnd, &ps);

      DrawBezier(hdc, apt);

      EndPaint(hwnd, &ps);
      return 0;

   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(hwnd, message, wParam, lParam);
}