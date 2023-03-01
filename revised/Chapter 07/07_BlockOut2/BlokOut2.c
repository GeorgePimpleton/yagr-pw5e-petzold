/*---------------------------------------------------
   BLOKOUT2.C -- Mouse Button & Capture Demo Program
             (c) Charles Petzold, 1998
  ---------------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(_In_     HINSTANCE instance,
                    _In_opt_ HINSTANCE prevInstance,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInstance);
   UNREFERENCED_PARAMETER(cmdLine);

   static PCWSTR  appName = L"BlokOut2";
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

   hwnd = CreateWindowW(appName, L"Mouse Button & Capture Demo",
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

void DrawBoxOutline(HWND hwnd, POINT ptBeg, POINT ptEnd)
{
   HDC hdc;

   hdc = GetDC(hwnd);

   SetROP2(hdc, R2_NOT);
   SelectObject(hdc, GetStockObject(NULL_BRUSH));
   Rectangle(hdc, ptBeg.x, ptBeg.y, ptEnd.x, ptEnd.y);

   ReleaseDC(hwnd, hdc);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   static BOOL  blocking;
   static BOOL  validBox;
   static POINT ptBeg;
   static POINT ptEnd;
   static POINT ptBoxBeg;
   static POINT ptBoxEnd;
   HDC          hdc;
   PAINTSTRUCT  ps;

   switch ( message )
   {
   case WM_LBUTTONDOWN:
      ptBeg.x = ptEnd.x = LOWORD(lParam);
      ptBeg.y = ptEnd.y = HIWORD(lParam);

      DrawBoxOutline(hwnd, ptBeg, ptEnd);

      SetCapture(hwnd);
      SetCursor((HCURSOR) LoadImageW(NULL, IDC_CROSS, IMAGE_CURSOR, 0, 0, LR_SHARED));

      blocking = TRUE;
      return 0;

   case WM_MOUSEMOVE:
      if ( blocking )
      {
         SetCursor((HCURSOR) LoadImageW(NULL, IDC_CROSS, IMAGE_CURSOR, 0, 0, LR_SHARED));

         DrawBoxOutline(hwnd, ptBeg, ptEnd);

         ptEnd.x = LOWORD(lParam);
         ptEnd.y = HIWORD(lParam);

         DrawBoxOutline(hwnd, ptBeg, ptEnd);
      }
      return 0;

   case WM_LBUTTONUP:
      if ( blocking )
      {
         DrawBoxOutline(hwnd, ptBeg, ptEnd);

         ptBoxBeg   = ptBeg;
         ptBoxEnd.x = LOWORD(lParam);
         ptBoxEnd.y = HIWORD(lParam);

         ReleaseCapture();
         SetCursor((HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED));

         blocking = FALSE;
         validBox = TRUE;

         InvalidateRect(hwnd, NULL, TRUE);
      }
      return 0;

   case WM_CHAR:
      if ( blocking & (wParam == '\x1B') )     // i.e., Escape
      {
         DrawBoxOutline(hwnd, ptBeg, ptEnd);

         ReleaseCapture();
         SetCursor((HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED));

         blocking = FALSE;
      }
      return 0;

   case WM_PAINT:
      hdc = BeginPaint(hwnd, &ps);

      if ( validBox )
      {
         SelectObject(hdc, GetStockObject(BLACK_BRUSH));
         Rectangle(hdc, ptBoxBeg.x, ptBoxBeg.y,
                   ptBoxEnd.x, ptBoxEnd.y);
      }

      if ( blocking )
      {
         SetROP2(hdc, R2_NOT);
         SelectObject(hdc, GetStockObject(NULL_BRUSH));
         Rectangle(hdc, ptBeg.x, ptBeg.y, ptEnd.x, ptEnd.y);
      }

      EndPaint(hwnd, &ps);
      return 0;

   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(hwnd, message, wParam, lParam);
}