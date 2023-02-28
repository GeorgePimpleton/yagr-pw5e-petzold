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

int WINAPI wWinMain(_In_     HINSTANCE instance,
                    _In_opt_ HINSTANCE prevInstance,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInstance);
   UNREFERENCED_PARAMETER(cmdLine);

   static PCWSTR  appName = L"RandRect";
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

   hwnd = CreateWindowW(appName, L"Random Rectangles",
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        NULL, NULL, instance, NULL);

   ShowWindow(hwnd, showCmd);
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