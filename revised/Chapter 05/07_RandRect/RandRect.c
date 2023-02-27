/*------------------------------------------
   RANDRECT.C -- Displays Random Rectangles
             (c) Charles Petzold, 1998
  ------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>     // for the rand function

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DrawRectangle(HWND);

int cxClient;
int cyClient;

int WINAPI wWinMain(_In_     HINSTANCE hInstance,
                    _In_opt_ HINSTANCE hPrevInstance,
                    _In_     PWSTR     pCmdLine,
                    _In_     int       nShowCmd)
{
   UNREFERENCED_PARAMETER(hPrevInstance);
   UNREFERENCED_PARAMETER(pCmdLine);

   static PCWSTR  szAppName = L"RandRect";
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

   hwnd = CreateWindowW(szAppName, L"Random Rectangles",
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        NULL, NULL, hInstance, NULL);

   ShowWindow(hwnd, nShowCmd);
   UpdateWindow(hwnd);

   while ( TRUE )
   {
      if ( PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE) )
      {
         if ( msg.message == WM_QUIT )
            break;

         TranslateMessage(&msg);
         DispatchMessageW(&msg);
      }
      else
      {
         DrawRectangle(hwnd);

         // slow down the rect drawing
         // but not enough to make Windows sluggish
         Sleep(1);
      }
   }

   return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
   switch ( iMsg )
   {
   case WM_SIZE:
      cxClient = GET_X_LPARAM(lParam);
      cyClient = GET_Y_LPARAM(lParam);
      return 0;

   case WM_DESTROY:PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(hwnd, iMsg, wParam, lParam);
}

void DrawRectangle(HWND hwnd)
{
   HBRUSH hBrush;
   HDC hdc;
   RECT rect;

   if ( cxClient == 0 || cyClient == 0 )
      return;

   SetRect(&rect, rand() % cxClient, rand() % cyClient,
           rand() % cxClient, rand() % cyClient);

   hBrush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));

   hdc = GetDC(hwnd);

   FillRect(hdc, &rect, hBrush);
   ReleaseDC(hwnd, hdc);
   DeleteObject(hBrush);
}