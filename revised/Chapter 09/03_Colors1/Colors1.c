/*----------------------------------------
   COLORS1.C -- Colors Using Scroll Bars
            (c) Charles Petzold, 1998
  ----------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#pragma comment(lib, "comctl32.lib")

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ScrollProc(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);

#define N (3)  // Number of colors.

int focus;

int WINAPI wWinMain(_In_     HINSTANCE instance,
                    _In_opt_ HINSTANCE prevInstance,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInstance);
   UNREFERENCED_PARAMETER(cmdLine);

   static PCWSTR  appName = L"Colors1";
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
   wc.hbrBackground = CreateSolidBrush(0);
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = appName;

   if ( !RegisterClassW(&wc) )
   {
      MessageBoxW(NULL, L"This program requires Windows NT!",
                  appName, MB_ICONERROR);
      return 0;
   }

   hwnd = CreateWindowW(appName, L"Color Scroll",
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
   static COLORREF prim[ N ] = { RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255) };
   static HBRUSH   brush[ N ];
   static HBRUSH   brushStatic;
   static HWND     scroll[ N ];
   static HWND     label[ N ];
   static HWND     value[ N ];
   static HWND     rect;
   static int      color[ N ];
   static int      cyChar;
   static RECT     rcColor;
   static PCWSTR   colorLabel[] = { L"Red", L"Green", L"Blue" };
   HINSTANCE       instance;
   int             i;
   int             cxClient;
   int             cyClient;
   WCHAR           buffer[ 10 ];

   switch ( message )
   {
   case WM_CREATE:
      instance = GetWindowInstance(hwnd);

      // create the white-rectangle window against which the
      // scroll bars will be positioned. The child window ID is 9.
      rect = CreateWindowW(L"static", NULL,
                           WS_CHILD | WS_VISIBLE | SS_WHITERECT,
                           0, 0, 0, 0,
                           hwnd, (HMENU) 9, instance, NULL);

      for ( i = 0; i < N; i++ )
      {
         // the three scroll bars have IDs 0, 1, and 2, with
         // scroll bar ranges from 0 through 255.
         scroll[ i ] = CreateWindowW(L"scrollbar", NULL,
                                     WS_CHILD | WS_VISIBLE |
                                     WS_TABSTOP | SBS_VERT,
                                     0, 0, 0, 0,
                                     hwnd, (HMENU) i, instance, NULL);

         SetScrollRange(scroll[ i ], SB_CTL, 0, 255, FALSE);
         SetScrollPos(scroll[ i ], SB_CTL, 0, FALSE);

         // the three color-name labels have IDs 3, 4, and 5,
         // and text strings "Red", "Green", and "Blue".
         label[ i ] = CreateWindowW(L"static", colorLabel[ i ],
                                    WS_CHILD | WS_VISIBLE | SS_CENTER,
                                    0, 0, 0, 0,
                                    hwnd, (HMENU) (i + N),
                                    instance, NULL);

         // the three color-value text fields have IDs 6, 7,
         // and 8, and initial text strings of "0".
         value[ i ] = CreateWindowW(L"static", L"0",
                                    WS_CHILD | WS_VISIBLE | SS_CENTER,
                                    0, 0, 0, 0,
                                    hwnd, (HMENU) (i + 2 * N),
                                    instance, NULL);

         SetWindowSubclass(scroll[ i ], ScrollProc, (UINT_PTR) i, 0);

         brush[ i ] = CreateSolidBrush(prim[ i ]);
      }

      brushStatic = CreateSolidBrush(GetSysColor(COLOR_BTNHIGHLIGHT));

      cyChar = HIWORD(GetDialogBaseUnits());
      return 0;

   case WM_SIZE:
      cxClient = GET_X_LPARAM(lParam);
      cyClient = GET_Y_LPARAM(lParam);

      SetRect(&rcColor, cxClient / 2, 0, cxClient, cyClient);

      MoveWindow(rect, 0, 0, cxClient / 2, cyClient, TRUE);

      for ( i = 0; i < N; i++ )
      {
         MoveWindow(scroll[ i ],
                    (2 * i + 1) * cxClient / 14, 2 * cyChar,
                    cxClient / 14, cyClient - 4 * cyChar, TRUE);

         MoveWindow(label[ i ],
                    (4 * i + 1) * cxClient / 28, cyChar / 2,
                    cxClient / 7, cyChar, TRUE);

         MoveWindow(value[ i ],
                    (4 * i + 1) * cxClient / 28,
                    cyClient - 3 * cyChar / 2,
                    cxClient / 7, cyChar, TRUE);
      }
      SetFocus(hwnd);
      return 0;

   case WM_SETFOCUS:
      SetFocus(scroll[ focus ]);
      return 0;

   case WM_VSCROLL:
      i = (int) GetWindowLongPtrW((HWND) lParam, GWLP_ID);

      switch ( LOWORD(wParam) )
      {
      case SB_PAGEDOWN:
         color[ i ] += 15;
         // fall through

      case SB_LINEDOWN:
         color[ i ] = min(255, color[ i ] + 1);
         break;

      case SB_PAGEUP:
         color[ i ] -= 15;
         // fall through

      case SB_LINEUP:
         color[ i ] = max(0, color[ i ] - 1);
         break;

      case SB_TOP:
         color[ i ] = 0;
         break;

      case SB_BOTTOM:
         color[ i ] = 255;
         break;

      case SB_THUMBPOSITION:
      case SB_THUMBTRACK:
         color[ i ] = HIWORD(wParam);
         break;

      default:
         break;
      }

      SetScrollPos(scroll[ i ], SB_CTL, color[ i ], TRUE);
      wsprintfW(buffer, L"%i", color[ i ]);
      SetWindowTextW(value[ i ], buffer);

      DeleteBrush((HBRUSH) SetClassLongPtrW(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)
                                            CreateSolidBrush(RGB(color[ 0 ], color[ 1 ], color[ 2 ]))));

      InvalidateRect(hwnd, &rcColor, TRUE);
      return 0;

   case WM_CTLCOLORSCROLLBAR:
      i = (int) GetWindowLongPtrW((HWND) lParam, GWLP_ID);
      return (LRESULT) brush[ i ];

   case WM_CTLCOLORSTATIC:
      i = (int) GetWindowLongPtrW((HWND) lParam, GWLP_ID);

      if ( i >= N && i <= 8 )    // static text controls
      {
         SetTextColor((HDC) wParam, prim[ i % N ]);
         SetBkColor((HDC) wParam, GetSysColor(COLOR_BTNHIGHLIGHT));
         return (LRESULT) brushStatic;
      }
      break;

   case WM_SYSCOLORCHANGE:
      DeleteBrush(brushStatic);
      brushStatic = CreateSolidBrush(GetSysColor(COLOR_BTNHIGHLIGHT));
      return 0;

   case WM_DESTROY:
      DeleteBrush((HBRUSH) SetClassLongPtrW(hwnd, GCLP_HBRBACKGROUND,
                                            (LONG_PTR) GetStockBrush(WHITE_BRUSH)));

      for ( i = 0; i < N; i++ )
      {
         DeleteBrush(brush[ i ]);
      }

      DeleteBrush(brushStatic);
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK ScrollProc(HWND      hwnd,
                            UINT      msg,
                            WPARAM    wParam,
                            LPARAM    lParam,
                            UINT_PTR  subclass,
                            DWORD_PTR refData)
{
   UNREFERENCED_PARAMETER(subclass);
   UNREFERENCED_PARAMETER(refData);

   int id = (int) GetWindowLongPtrW(hwnd, GWLP_ID);

   switch ( msg )
   {
   case WM_KEYDOWN:
      if ( wParam == VK_TAB )
      {
         SetFocus(GetDlgItem(GetParent(hwnd), (id + (GetKeyState(VK_SHIFT) < 0 ? 2 : 1)) % N));
      }
      break;

   case WM_NCDESTROY:
      RemoveWindowSubclass(hwnd, ScrollProc, id);
      break;

   case WM_SETFOCUS:
      focus = id;
      break;
   }
   return DefSubclassProc(hwnd, msg, wParam, lParam);
}