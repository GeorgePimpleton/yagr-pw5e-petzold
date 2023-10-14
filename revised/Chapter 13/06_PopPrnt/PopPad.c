/*---------------------------------------
   POPPAD.C -- Popup Editor
               (c) Charles Petzold, 1998
  ---------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include "Resource.h"
#include "PopPad.h"

#define EDITID   1
#define UNTITLED L"(untitled)"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);

// Global variables
static HWND  dlgModeless;
static PCWSTR appName = L"PopPad";

int WINAPI wWinMain(_In_     HINSTANCE inst,
                    _In_opt_ HINSTANCE prevInst,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInst);

   MSG       msg;
   HWND      wnd;
   HACCEL    accel;
   WNDCLASS  wc;

   wc.style         = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = inst;
   wc.hIcon         = (HICON)   LoadImageW(inst, appName, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wc.hbrBackground = (HBRUSH)  (COLOR_WINDOW + 1);
   wc.lpszMenuName  = appName;
   wc.lpszClassName = appName;

   if ( !RegisterClassW(&wc) )
   {
      MessageBoxW(NULL, L"This program requires Windows NT!", appName, MB_ICONERROR);
      return 0;
   }

   wnd = CreateWindowW(appName, NULL,
                       WS_OVERLAPPEDWINDOW,
                       CW_USEDEFAULT, CW_USEDEFAULT,
                       CW_USEDEFAULT, CW_USEDEFAULT,
                       NULL, NULL, inst, cmdLine);

   ShowWindow(wnd, showCmd);
   UpdateWindow(wnd);

   accel = LoadAcceleratorsW(inst, appName);

   while ( GetMessageW(&msg, NULL, 0, 0) )
   {
      if ( dlgModeless == NULL || !IsDialogMessageW(dlgModeless, &msg) )
      {
         if ( !TranslateAcceleratorW(wnd, accel, &msg) )
         {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
         }
      }
   }
   return (int) msg.wParam;
}

void DoCaption(HWND wnd, PCWSTR titleName)
{
   WCHAR caption[ 64 + MAX_PATH ];

   wsprintfW(caption, L"%s - %s", appName,
             titleName[ 0 ] ? titleName : UNTITLED);

   SetWindowTextW(wnd, caption);
}

void OkMessage(HWND wnd, PCWSTR message, PCWSTR titleName)
{
   WCHAR buffer[ 64 + MAX_PATH ];

   wsprintfW(buffer, message, titleName[ 0 ] ? titleName : UNTITLED);

   MessageBoxW(wnd, buffer, appName, MB_OK | MB_ICONEXCLAMATION);
}

short AskAboutSave(HWND wnd, WCHAR* titleName)
{
   WCHAR buffer[ 64 + MAX_PATH ];
   int   ret;

   wsprintfW(buffer, L"Save current changes in %s?",
             titleName[ 0 ] ? titleName : UNTITLED);

   ret = MessageBoxW(wnd, buffer, appName,
                     MB_YESNOCANCEL | MB_ICONQUESTION);

   if ( ret == IDYES )
   {
      if ( !SendMessage(wnd, WM_COMMAND, IDM_FILE_SAVE, 0) )
      {
         ret = IDCANCEL;
      }
   }

   return ret;
}

LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static BOOL      needSave = FALSE;
   static HINSTANCE inst;
   static HWND      wndEdit;
   static int       offset;
   static WCHAR     fileName[ MAX_PATH ];
   static WCHAR     titleName[ MAX_PATH ];
   static UINT      messageFindReplace;
   int              selBeg;
   int              selEnd;
   int              enable;
   LPFINDREPLACEW   fr;

   switch ( msg )
   {
   case WM_CREATE:
      inst = GetWindowInstance(wnd);

      // Create the edit control child window
      wndEdit = CreateWindowW(L"edit", NULL,
                              WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
                              WS_BORDER | ES_LEFT | ES_MULTILINE |
                              ES_NOHIDESEL | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
                              0, 0, 0, 0,
                              wnd, (HMENU) EDITID, inst, NULL);

      SendMessageW(wndEdit, EM_LIMITTEXT, 32000, 0L);

      // Initialize common dialog box stuff
      PopFileInitialize(wnd);
      PopFontInitialize(wndEdit);

      messageFindReplace = RegisterWindowMessageW(FINDMSGSTRINGW);

      DoCaption(wnd, titleName);
      return 0;

   case WM_SETFOCUS:
      SetFocus(wndEdit);
      return 0;

   case WM_SIZE:
      MoveWindow(wndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
      return 0;

   case WM_INITMENUPOPUP:
      switch ( lParam )
      {
      case 1:             // Edit menu

         // Enable Undo if edit control can do it
         EnableMenuItem((HMENU) wParam, IDM_EDIT_UNDO,
                        SendMessageW(wndEdit, EM_CANUNDO, 0, 0L) ?
                                     MF_ENABLED : MF_GRAYED);

         // Enable Paste if text is in the clipboard

         EnableMenuItem((HMENU) wParam, IDM_EDIT_PASTE,
                        IsClipboardFormatAvailable(CF_TEXT) ?
                        MF_ENABLED : MF_GRAYED);

         // Enable Cut, Copy, and Del if text is selected
         SendMessageW(wndEdit, EM_GETSEL, (WPARAM) &selBeg,
                     (LPARAM) &selEnd);

         enable = selBeg != selEnd ? MF_ENABLED : MF_GRAYED;

         EnableMenuItem((HMENU) wParam, IDM_EDIT_CUT, enable);
         EnableMenuItem((HMENU) wParam, IDM_EDIT_COPY, enable);
         EnableMenuItem((HMENU) wParam, IDM_EDIT_CLEAR, enable);
         break;

      case 2:             // Search menu

         // Enable Find, Next, and Replace if modeless
         //   dialogs are not already active
         enable = dlgModeless == NULL ? MF_ENABLED : MF_GRAYED;

         EnableMenuItem((HMENU) wParam, IDM_SEARCH_FIND, enable);
         EnableMenuItem((HMENU) wParam, IDM_SEARCH_NEXT, enable);
         EnableMenuItem((HMENU) wParam, IDM_SEARCH_REPLACE, enable);
         break;
      }
      return 0;

   case WM_COMMAND:
      // Messages from edit control
      if ( lParam && LOWORD(wParam) == EDITID )
      {
         switch ( HIWORD(wParam) )
         {
         case EN_UPDATE:
            needSave = TRUE;
            return 0;

         case EN_ERRSPACE:
         case EN_MAXTEXT:
            MessageBoxW(wnd, L"Edit control out of space.", appName, MB_OK | MB_ICONSTOP);
            return 0;
         }
         break;
      }

      switch ( LOWORD(wParam) )
      {
         // Messages from File menu
      case IDM_FILE_NEW:
         if ( needSave && IDCANCEL == AskAboutSave(wnd, titleName) )
         {
            return 0;
         }

         SetWindowTextW(wndEdit, L"\0");
         fileName[ 0 ]  = '\0';
         titleName[ 0 ] = '\0';
         DoCaption(wnd, titleName);
         needSave = FALSE;
         return 0;

      case IDM_FILE_OPEN:
         if ( needSave && IDCANCEL == AskAboutSave(wnd, titleName) )
         {
            return 0;
         }

         if ( PopFileOpenDlg(wnd, fileName, titleName) )
         {
            if ( !PopFileRead(wndEdit, fileName) )
            {
               OkMessage(wnd, L"Could not read file %s!", titleName);
               fileName[ 0 ]  = '\0';
               titleName[ 0 ] = '\0';
            }
         }

         DoCaption(wnd, titleName);
         needSave = FALSE;
         return 0;

      case IDM_FILE_SAVE:
         if ( fileName[ 0 ] )
         {
            if ( PopFileWrite(wndEdit, fileName) )
            {
               needSave = FALSE;
               return 1;
            }
            else
            {
               OkMessage(wnd, L"Could not write file %s", titleName);
               return 0;
            }
         }
         // fall through
      case IDM_FILE_SAVE_AS:
         if ( PopFileSaveDlg(wnd, fileName, titleName) )
         {
            DoCaption(wnd, titleName);

            if ( PopFileWrite(wndEdit, fileName) )
            {
               needSave = FALSE;
               return 1;
            }
            else
            {
               OkMessage(wnd, TEXT("Could not write file %s"),
                         titleName);
               return 0;
            }
         }
         return 0;

      case IDM_FILE_PRINT:
         if ( !PopPrntPrintFile(inst, wnd, wndEdit, titleName) )
         {
            OkMessage(wnd, L"Could not print file %s", titleName);
         }
         return 0;

      case IDM_APP_EXIT:
         SendMessageW(wnd, WM_CLOSE, 0, 0);
         return 0;

         // Messages from Edit menu

      case IDM_EDIT_UNDO:
         SendMessageW(wndEdit, WM_UNDO, 0, 0);
         return 0;

      case IDM_EDIT_CUT:
         SendMessageW(wndEdit, WM_CUT, 0, 0);
         return 0;

      case IDM_EDIT_COPY:
         SendMessageW(wndEdit, WM_COPY, 0, 0);
         return 0;

      case IDM_EDIT_PASTE:
         SendMessageW(wndEdit, WM_PASTE, 0, 0);
         return 0;

      case IDM_EDIT_CLEAR:
         SendMessageW(wndEdit, WM_CLEAR, 0, 0);
         return 0;

      case IDM_EDIT_SELECT_ALL:
         SendMessageW(wndEdit, EM_SETSEL, 0, -1);
         return 0;

         // Messages from Search menu

      case IDM_SEARCH_FIND:
         SendMessageW(wndEdit, EM_GETSEL, 0, (LPARAM) &offset);
         dlgModeless = PopFindFindDlg(wnd);
         return 0;

      case IDM_SEARCH_NEXT:
         SendMessageW(wndEdit, EM_GETSEL, 0, (LPARAM) &offset);

         if ( PopFindValidFind( ) )
         {
            PopFindNextText(wndEdit, &offset);
         }
         else
         {
            dlgModeless = PopFindFindDlg(wnd);
         }

         return 0;

      case IDM_SEARCH_REPLACE:
         SendMessageW(wndEdit, EM_GETSEL, 0, (LPARAM) &offset);
         dlgModeless = PopFindReplaceDlg(wnd);
         return 0;

      case IDM_FORMAT_FONT:
         if ( PopFontChooseFont(wnd) )
         {
            PopFontSetFont(wndEdit);
         }

         return 0;

         // Messages from Help menu

      case IDM_HELP:
         OkMessage(wnd, L"Help not yet implemented!", L"\0");
         return 0;

      case IDM_APP_ABOUT:
         DialogBoxParamW(inst, L"AboutBox", wnd, AboutDlgProc, 0L);
         return 0;
      }
      break;

   case WM_CLOSE:
      if ( !needSave || IDCANCEL != AskAboutSave(wnd, titleName) )
         DestroyWindow(wnd);

      return 0;

   case WM_QUERYENDSESSION:
      if ( !needSave || IDCANCEL != AskAboutSave(wnd, titleName) )
         return 1;

      return 0;

   case WM_DESTROY:
      PopFontDeinitialize( );
      PostQuitMessage(0);
      return 0;

   default:
      // Process "Find-Replace" messages
      if ( msg == messageFindReplace )
      {
         fr = (LPFINDREPLACEW) lParam;

         if ( fr->Flags & FR_DIALOGTERM )
         {
            dlgModeless = NULL;
         }

         if ( fr->Flags & FR_FINDNEXT )
         {
            if ( !PopFindFindText(wndEdit, &offset, fr) )
            {
               OkMessage(wnd, L"Text not found!", L"\0");
            }
         }

         if ( fr->Flags & FR_REPLACE || fr->Flags & FR_REPLACEALL )
         {
            if ( !PopFindReplaceText(wndEdit, &offset, fr) )
            {
               OkMessage(wnd, L"Text not found!", L"\0");
            }
         }

         if ( fr->Flags & FR_REPLACEALL )
         {
            while ( PopFindReplaceText(wndEdit, &offset, fr) ) ;
         }

         return 0;
      }
      break;
   }
   return DefWindowProcW(wnd, msg, wParam, lParam);
}

INT_PTR CALLBACK AboutDlgProc(HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
   UNREFERENCED_PARAMETER(lParam);

   switch ( msg )
   {
   case WM_INITDIALOG:
      return TRUE;

   case WM_COMMAND:
      switch ( GET_WM_COMMAND_ID(wParam, lParam) )
      {
      case IDOK:
         EndDialog(dlg, 0);
         return TRUE;
      }
      break;
   }
   return FALSE;
}
