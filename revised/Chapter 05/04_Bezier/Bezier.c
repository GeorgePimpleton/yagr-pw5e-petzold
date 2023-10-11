/*---------------------------------------
   BEZIER.C -- Bezier Splines Demo
               (c) Charles Petzold, 1998
  ---------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void             DrawBezier(HDC, POINT[ ]);

int WINAPI wWinMain(_In_     HINSTANCE inst,
                    _In_opt_ HINSTANCE prevInst,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInst);
   UNREFERENCED_PARAMETER(cmdLine);

   static PCWSTR  appName = L"Bezier";
   HWND           wnd;
   MSG            msg;
   WNDCLASSW      wc;

   wc.style         = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = inst;
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

   wnd = CreateWindowW(appName, L"Bezier Splines",
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

void DrawBezier(HDC dc, POINT apt[ ])
{
   PolyBezier(dc, apt, 4);

   MoveToEx(dc, apt[ 0 ].x, apt[ 0 ].y, NULL);
   LineTo(dc, apt[ 1 ].x, apt[ 1 ].y);

   MoveToEx(dc, apt[ 2 ].x, apt[ 2 ].y, NULL);
   LineTo(dc, apt[ 3 ].x, apt[ 3 ].y);
}

LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static POINT   apt[ 4 ];
   int            xClient;
   int            yClient;
   HDC            dc;
   PAINTSTRUCT    ps;

   switch ( msg )
   {
   case WM_SIZE:
      xClient = GET_X_LPARAM(lParam);
      yClient = GET_Y_LPARAM(lParam);

      apt[ 0 ].x = xClient / 4;
      apt[ 0 ].y = yClient / 2;

      apt[ 1 ].x = xClient / 2;
      apt[ 1 ].y = yClient / 4;

      apt[ 2 ].x = xClient / 2;
      apt[ 2 ].y = 3 * yClient / 4;

      apt[ 3 ].x = 3 * xClient / 4;
      apt[ 3 ].y = yClient / 2;

      return 0;

   case WM_LBUTTONDOWN:
   case WM_RBUTTONDOWN:
   case WM_MOUSEMOVE:
      if ( wParam & MK_LBUTTON || wParam & MK_RBUTTON )
      {
         dc = GetDC(wnd);

         SelectObject(dc, GetStockObject(WHITE_PEN));
         DrawBezier(dc, apt);

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

         SelectObject(dc, GetStockObject(BLACK_PEN));
         DrawBezier(dc, apt);
         ReleaseDC(wnd, dc);
      }
      return 0;

   case WM_PAINT:
      InvalidateRect(wnd, NULL, TRUE);

      dc = BeginPaint(wnd, &ps);

      DrawBezier(dc, apt);

      EndPaint(wnd, &ps);
      return 0;

   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(wnd, msg, wParam, lParam);
}