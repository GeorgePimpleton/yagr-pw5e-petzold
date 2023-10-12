/*----------------------------------------
   ENVIRON.C -- Environment List Box
                (c) Charles Petzold, 1998
  ----------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <malloc.h>

#define ID_LIST     1
#define ID_TEXT     2

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(_In_     HINSTANCE inst,
                    _In_opt_ HINSTANCE prevInst,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInst);
   UNREFERENCED_PARAMETER(cmdLine);

   static PCWSTR appName = L"Environ";
   HWND          wnd;
   MSG           msg;
   WNDCLASSW     wc;

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

   wnd = CreateWindowW(appName, L"Environment List Box",
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

void FillListBox(HWND wndList)
{
   int    length;
   WCHAR* EBlock;
   WCHAR* varBlock;
   WCHAR* varBeg;
   WCHAR* varEnd;
   WCHAR* varName;

   EBlock = (WCHAR*) GetEnvironmentStringsW( );  // Get pointer to environment block
   varBlock = EBlock;

   while ( *varBlock )
   {
      if ( *varBlock != '=' )   // Skip variable names beginning with '='
      {
         varBeg = varBlock;                // Beginning of variable name
         while ( *varBlock++ != L'=' ) ;   // Scan until '='
         varEnd = varBlock - 1;            // Points to '=' sign
         length = (int) (varEnd - varBeg); // Length of variable name

         // Allocate memory for the variable name and terminating
         // zero. Copy the variable name and append a zero.

         varName = (WCHAR*) calloc(length + 1, sizeof(WCHAR));
         CopyMemory(varName, varBeg, length * sizeof(WCHAR));
         varName[ length ] = L'\0';

         // Put the variable name in the list box and free memory.
         SendMessageW(wndList, LB_ADDSTRING, 0, (LPARAM) varName);
         free(varName);
      }
      while ( *varBlock++ != '\0' );     // Scan until terminating zero
   }
   FreeEnvironmentStringsW(EBlock);
}

LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static HWND list;
   static HWND text;
   int         index;
   int         length;
   int         xChar;
   int         yChar;
   WCHAR* varName;
   WCHAR* varValue;

   switch ( msg )
   {
   case WM_CREATE:
      xChar = LOWORD(GetDialogBaseUnits( ));
      yChar = HIWORD(GetDialogBaseUnits( ));

      // Create listbox and static text windows.

      list = CreateWindowW(L"listbox", NULL,
                           WS_CHILD | WS_VISIBLE | LBS_STANDARD,
                           xChar, yChar * 3,
                           xChar * 16 + GetSystemMetrics(SM_CXVSCROLL),
                           yChar * 5,
                           wnd, (HMENU) ID_LIST,
                           GetWindowInstance(wnd),
                           NULL);

      text = CreateWindowW(L"static", NULL,
                           WS_CHILD | WS_VISIBLE | SS_LEFT,
                           xChar, yChar,
                           GetSystemMetrics(SM_CXSCREEN), yChar,
                           wnd, (HMENU) ID_TEXT,
                           GetWindowInstance(wnd),
                           NULL);

      FillListBox(list);
      return 0;

   case WM_SETFOCUS:
      SetFocus(list);
      return 0;

   case WM_COMMAND:
      if ( LOWORD(wParam) == ID_LIST && HIWORD(wParam) == LBN_SELCHANGE )
      {
         // Get current selection.

         index = ListBox_GetCurSel(list);
         length = ListBox_GetTextLen(list, index) + 1;
         varName = (WCHAR*) calloc(length, sizeof(WCHAR));
         ListBox_GetText(list, index, varName);

         // Get environment string.

         length = GetEnvironmentVariableW(varName, NULL, 0);
         varValue = (WCHAR*) calloc(length, sizeof(WCHAR));
         GetEnvironmentVariableW(varName, varValue, length);

         // Show it in window.

         SetWindowTextW(text, varValue);
         free(varName);
         free(varValue);
      }
      return 0;

   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(wnd, msg, wParam, lParam);
}