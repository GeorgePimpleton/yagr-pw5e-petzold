/*-------------------------------------------------
   NOPOPUPS.C -- Demonstrates No-Popup Nested Menu
             (c) Charles Petzold, 1998
  -------------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "Resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(_In_     HINSTANCE instance,
                    _In_opt_ HINSTANCE prevInstance,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInstance);
   UNREFERENCED_PARAMETER(cmdLine);

   static PCWSTR appName = L"NoPopUps";
   HWND          hwnd;
   MSG           msg;
   WNDCLASSW     wc;

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

   hwnd = CreateWindowW(appName,
                        L"No-Popup Nested Menu Demonstration",
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
   static HMENU menuMain;
   static HMENU menuEdit;
   static HMENU menuFile;
   HINSTANCE    instance;

   switch ( message )
   {
   case WM_CREATE:
      instance = (HINSTANCE) GetWindowLongPtrW(hwnd, GWLP_HINSTANCE);

      menuMain = LoadMenuW(instance, L"MenuMain");
      menuFile = LoadMenuW(instance, L"MenuFile");
      menuEdit = LoadMenuW(instance, L"MenuEdit");

      SetMenu(hwnd, menuMain);
      return 0;

   case WM_COMMAND:
      switch ( LOWORD(wParam) )
      {
      case IDM_MAIN:
         SetMenu(hwnd, menuMain);
         return 0;

      case IDM_FILE:
         SetMenu(hwnd, menuFile);
         return 0;

      case IDM_EDIT:
         SetMenu(hwnd, menuEdit);
         return 0;

      case IDM_FILE_NEW:
      case IDM_FILE_OPEN:
      case IDM_FILE_SAVE:
      case IDM_FILE_SAVE_AS:
      case IDM_EDIT_UNDO:
      case IDM_EDIT_CUT:
      case IDM_EDIT_COPY:
      case IDM_EDIT_PASTE:
      case IDM_EDIT_CLEAR:
         MessageBeep(0);
         return 0;
      }
      break;

   case WM_DESTROY:
      SetMenu(hwnd, menuMain);
      DestroyMenu(menuFile);
      DestroyMenu(menuEdit);

      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(hwnd, message, wParam, lParam);
}