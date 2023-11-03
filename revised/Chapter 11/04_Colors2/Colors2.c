/*------------------------------------------------
   COLORS2.C -- Version using Modeless Dialog Box
                (c) Charles Petzold, 1998
  ------------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK ColorScrDlg(HWND, UINT, WPARAM, LPARAM);

HWND dlgModeless;

int WINAPI wWinMain(_In_     HINSTANCE inst,
                    _In_opt_ HINSTANCE prevInst,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInst);
   UNREFERENCED_PARAMETER(cmdLine);

   PCWSTR    appName = L"Colors2";
   HWND      wnd;
   MSG       msg;
   WNDCLASSW wc      = { 0 };

   wc.style         = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = inst;
   wc.hIcon         = (HICON)   LoadImageW(inst, appName, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wc.hbrBackground = CreateSolidBrush(0L);
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = appName;

   if ( !RegisterClassW(&wc) )
   {
      MessageBoxW(NULL, L"This program requires Windows NT!", appName, MB_ICONERROR);
      return 0;
   }

   wnd = CreateWindowW(appName, L"Color Scroll",
                       WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                       CW_USEDEFAULT, CW_USEDEFAULT,
                       CW_USEDEFAULT, CW_USEDEFAULT,
                       NULL, NULL, inst, NULL);

   ShowWindow(wnd, showCmd);
   UpdateWindow(wnd);

   dlgModeless = CreateDialogParamW(inst, L"ColorScrDlg",
                                    wnd, ColorScrDlg, 0L);

   while ( GetMessageW(&msg, NULL, 0, 0) )
   {
      if ( dlgModeless == 0 || !IsDialogMessageW(dlgModeless, &msg) )
      {
         TranslateMessage(&msg);
         DispatchMessageW(&msg);
      }
   }
   return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch ( msg )
   {
   case WM_DESTROY:
      DeleteObject((HGDIOBJ) SetClassLongPtrW(wnd,
                                              GCLP_HBRBACKGROUND,
                                              (LONG_PTR) GetStockObject(WHITE_BRUSH)));
      PostQuitMessage(0);
      return 0;
   }
   return DefWindowProcW(wnd, msg, wParam, lParam);
}

INT_PTR CALLBACK ColorScrDlg(HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static int color[ 3 ];
   HWND       wndParent;
   HWND       ctrl;
   int        ctrlID;
   int        index;

   switch ( msg )
   {
   case WM_INITDIALOG:
      for ( ctrlID = 10; ctrlID < 13; ctrlID++ )
      {
         ctrl = GetDlgItem(dlg, ctrlID);
         SetScrollRange(ctrl, SB_CTL, 0, 255, FALSE);
         SetScrollPos(ctrl, SB_CTL, 0, FALSE);
      }
      return TRUE;

   case WM_VSCROLL:
      ctrl      = (HWND) lParam;
      ctrlID    = (int) GetWindowLongPtrW(ctrl, GWLP_ID);
      index     = ctrlID - 10;
      wndParent = GetParent(dlg);

      switch ( LOWORD(wParam) )
      {
      case SB_PAGEDOWN:
         color[ index ] += 15;
         // fall through

      case SB_LINEDOWN:
         color[ index ] = min(255, color[ index ] + 1);
         break;

      case SB_PAGEUP:
         color[ index ] -= 15;
         // fall through

      case SB_LINEUP:
         color[ index ] = max(0, color[ index ] - 1);
         break;

      case SB_TOP:
         color[ index ] = 0;
         break;

      case SB_BOTTOM:
         color[ index ] = 255;
         break;

      case SB_THUMBPOSITION:
      case SB_THUMBTRACK:
         color[ index ] = HIWORD(wParam);
         break;

      default:
         return FALSE;
      }
      SetScrollPos(ctrl, SB_CTL, color[ index ], TRUE);
      SetDlgItemInt(dlg, ctrlID + 3, color[ index ], FALSE);

      DeleteObject((HGDIOBJ) SetClassLongPtrW(wndParent,
                                              GCLP_HBRBACKGROUND,
                                              (LONG_PTR) CreateSolidBrush(RGB(color[ 0 ], color[ 1 ], color[ 2 ]))));

      InvalidateRect(wndParent, NULL, TRUE);
      return TRUE;
   }
   return FALSE;
}