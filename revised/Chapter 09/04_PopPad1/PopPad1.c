/*-------------------------------------------------------
   POPPAD1.C -- Popup Editor using child window edit box
            (c) Charles Petzold, 1998
  -------------------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>

#define ID_EDIT     1

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

PCWSTR appName = L"PopPad1";

int WINAPI wWinMain(_In_     HINSTANCE instance,
                    _In_opt_ HINSTANCE prevInstance,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInstance);
   UNREFERENCED_PARAMETER(cmdLine);

   HWND      hwnd;
   MSG       msg;
   WNDCLASSW wc;

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

   hwnd = CreateWindowW(appName, appName,
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

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   static HWND edit;

   switch ( message )
   {
   case WM_CREATE:
      edit = CreateWindowW(L"edit", NULL,
                           WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
                           WS_BORDER | ES_LEFT | ES_MULTILINE |
                           ES_AUTOHSCROLL | ES_AUTOVSCROLL,
                           0, 0, 0, 0, hwnd, (HMENU) ID_EDIT,
                           ((LPCREATESTRUCT) lParam)->hInstance, NULL);
      return 0;

   case WM_SETFOCUS:
      SetFocus(edit);
      return 0;

   case WM_SIZE:
      MoveWindow(edit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
      return 0;

   case WM_COMMAND:
      if ( LOWORD(wParam) == ID_EDIT )
         if ( HIWORD(wParam) == EN_ERRSPACE ||
              HIWORD(wParam) == EN_MAXTEXT )

            MessageBoxW(hwnd, L"Edit control out of space.",
                        appName, MB_OK | MB_ICONSTOP);
      return 0;

   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(hwnd, message, wParam, lParam);
}