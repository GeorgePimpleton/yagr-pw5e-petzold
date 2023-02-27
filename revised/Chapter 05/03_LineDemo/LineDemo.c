/*--------------------------------------------------
   LINEDEMO.C -- Line-Drawing Demonstration Program
             (c) Charles Petzold, 1998
  --------------------------------------------------*/

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

   static PCWSTR szAppName = L"LineDemo";
   HWND          hwnd;
   MSG           msg;
   WNDCLASSW     wc;

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
      MessageBoxW(NULL, L"Program requires Windows NT!",
                  szAppName, MB_ICONERROR);
      return 0;
   }

   hwnd = CreateWindowW(szAppName, L"Line Demonstration",
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
   static int  cxClient;
   static int  cyClient;
   HDC         hdc;
   PAINTSTRUCT ps;

   switch ( message )
   {
   case WM_SIZE:
      cxClient = GET_X_LPARAM(lParam);
      cyClient = GET_Y_LPARAM(lParam);
      return 0;

   case WM_PAINT:
      hdc = BeginPaint(hwnd, &ps);

      Rectangle(hdc, cxClient / 8, cyClient / 8,
                7 * cxClient / 8, 7 * cyClient / 8);

      MoveToEx(hdc, 0, 0, NULL);
      LineTo(hdc, cxClient, cyClient);

      MoveToEx(hdc, 0, cyClient, NULL);
      LineTo(hdc, cxClient, 0);

      Ellipse(hdc, cxClient / 8, cyClient / 8,
              7 * cxClient / 8, 7 * cyClient / 8);

      RoundRect(hdc, cxClient / 4, cyClient / 4,
                3 * cxClient / 4, 3 * cyClient / 4,
                cxClient / 4, cyClient / 4);

      EndPaint(hwnd, &ps);
      return 0;

   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(hwnd, message, wParam, lParam);
}