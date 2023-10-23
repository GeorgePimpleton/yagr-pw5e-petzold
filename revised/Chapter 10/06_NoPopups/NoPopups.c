/*-------------------------------------------------
   NOPOPUPS.C -- Demonstrates No-Popup Nested Menu
                 (c) Charles Petzold, 1998
  -------------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include "Resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(_In_     HINSTANCE inst,
                    _In_opt_ HINSTANCE prevInst,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInst);
   UNREFERENCED_PARAMETER(cmdLine);

   static PCWSTR appName = L"NoPopUps";
   HWND          wnd;
   MSG           msg;
   WNDCLASSW     wc      = { 0 };

   wc.style         = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = inst;
   wc.hIcon         = (HICON)   LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wc.hbrBackground = (HBRUSH)  (COLOR_WINDOW + 1);
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = appName;

   if ( !RegisterClassW(&wc) )
   {
      MessageBoxW(NULL, L"This program requires Windows NT!", appName, MB_ICONERROR);
      return 0;
   }

   wnd = CreateWindowW(appName,
                       L"No-Popup Nested Menu Demonstration",
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

LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static HMENU menuMain;
   static HMENU menuEdit;
   static HMENU menuFile;
   HINSTANCE    inst;

   switch ( msg )
   {
   case WM_CREATE:
      inst = GetWindowInstance(wnd);

      menuMain = LoadMenuW(inst, L"MenuMain");
      menuFile = LoadMenuW(inst, L"MenuFile");
      menuEdit = LoadMenuW(inst, L"MenuEdit");

      SetMenu(wnd, menuMain);
      return 0;

   case WM_COMMAND:
      switch ( LOWORD(wParam) )
      {
      case IDM_MAIN:
         SetMenu(wnd, menuMain);
         return 0;

      case IDM_FILE:
         SetMenu(wnd, menuFile);
         return 0;

      case IDM_EDIT:
         SetMenu(wnd, menuEdit);
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
      SetMenu(wnd, menuMain);
      DestroyMenu(menuFile);
      DestroyMenu(menuEdit);

      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(wnd, msg, wParam, lParam);
}