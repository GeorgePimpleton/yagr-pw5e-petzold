/*-----------------------------------------
   STOKFONT.C -- Stock Font Objects
             (c) Charles Petzold, 1998
  -----------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdlib.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(_In_     HINSTANCE hInstance,
                    _In_opt_ HINSTANCE hPrevInstance,
                    _In_     PWSTR     pCmdLine,
                    _In_     int       nShowCmd)
{
   UNREFERENCED_PARAMETER(hPrevInstance);
   UNREFERENCED_PARAMETER(pCmdLine);

   static PCWSTR appName = L"StokFont";
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
   wc.lpszClassName = appName;

   if ( !RegisterClassW(&wc) )
   {
      MessageBoxW(NULL, L"Program requires Windows NT!",
                  appName, MB_ICONERROR);
      return 0;
   }

   hwnd = CreateWindowW(appName, L"Stock Fonts",
                        WS_OVERLAPPEDWINDOW | WS_VSCROLL,
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
   static struct
   {
      int    idStockFont;
      PCWSTR szStockFont;
   }
   stockfont[] = { {OEM_FIXED_FONT,       L"OEM_FIXED_FONT"},
                   {ANSI_FIXED_FONT,      L"ANSI_FIXED_FONT"},
                   {ANSI_VAR_FONT,        L"ANSI_VAR_FONT"},
                   {SYSTEM_FONT,          L"SYSTEM_FONT"},
                   {DEVICE_DEFAULT_FONT,  L"DEVICE_DEFAULT_FONT"},
                   {SYSTEM_FIXED_FONT,    L"SYSTEM_FIXED_FONT"},
                   {DEFAULT_GUI_FONT,     L"DEFAULT_GUI_FONT"} };

   static int  iFont;
   static int  cFonts = _countof(stockfont);
   HDC         hdc;
   int         i;
   int         x;
   int         y;
   int         cxGrid;
   int         cyGrid;
   PAINTSTRUCT ps;
   WCHAR       szFaceName[ LF_FACESIZE ];
   WCHAR       szBuffer[ LF_FACESIZE + 64 ];
   TEXTMETRICW tm;

   switch ( message )
   {
   case WM_CREATE:
      SetScrollRange(hwnd, SB_VERT, 0, cFonts - 1, TRUE);
      return 0;

   case WM_DISPLAYCHANGE:
      InvalidateRect(hwnd, NULL, TRUE);
      return 0;

   case WM_VSCROLL:
      switch ( LOWORD(wParam) )
      {
      case SB_TOP:
         iFont = 0;
         break;

      case SB_BOTTOM:
         iFont = cFonts - 1;
         break;

      case SB_LINEUP:
      case SB_PAGEUP:
         iFont -= 1;
         break;

      case SB_LINEDOWN:
      case SB_PAGEDOWN:
         iFont += 1;
         break;

      case SB_THUMBPOSITION:
         iFont = HIWORD(wParam);
         break;
      }

      iFont = max(0, min(cFonts - 1, iFont));
      SetScrollPos(hwnd, SB_VERT, iFont, TRUE);
      InvalidateRect(hwnd, NULL, TRUE);
      return 0;

   case WM_KEYDOWN:
      switch ( wParam )
      {
      case VK_HOME:
         SendMessageW(hwnd, WM_VSCROLL, SB_TOP, 0);
         break;

      case VK_END:
         SendMessageW(hwnd, WM_VSCROLL, SB_BOTTOM, 0);
         break;

      case VK_PRIOR:
      case VK_LEFT:
      case VK_UP:
         SendMessageW(hwnd, WM_VSCROLL, SB_LINEUP, 0);
         break;

      case VK_NEXT:
      case VK_RIGHT:
      case VK_DOWN:
         SendMessageW(hwnd, WM_VSCROLL, SB_PAGEDOWN, 0);
         break;
      }
      return 0;

   case WM_PAINT:
      hdc = BeginPaint(hwnd, &ps);

      SelectObject(hdc, GetStockObject(stockfont[ iFont ].idStockFont));
      GetTextFaceW(hdc, LF_FACESIZE, szFaceName);
      GetTextMetricsW(hdc, &tm);

      cxGrid = max(3 * tm.tmAveCharWidth, 2 * tm.tmMaxCharWidth);
      cyGrid = tm.tmHeight + 3;

      TextOutW(hdc, 0, 0, szBuffer,
               wsprintfW(szBuffer, L" %s: Face Name = %s, CharSet = %i",
                         stockfont[ iFont ].szStockFont,
                         szFaceName, tm.tmCharSet));

      SetTextAlign(hdc, TA_TOP | TA_CENTER);

      // vertical and horizontal lines
      for ( i = 0; i < 17; i++ )
      {
         MoveToEx(hdc, (i + 2) * cxGrid, 2 * cyGrid, NULL);
         LineTo(hdc, (i + 2) * cxGrid, 19 * cyGrid);

         MoveToEx(hdc, cxGrid, (i + 3) * cyGrid, NULL);
         LineTo(hdc, 18 * cxGrid, (i + 3) * cyGrid);
      }

      // vertical and horizontal headings
      for ( i = 0; i < 16; i++ )
      {
         TextOutW(hdc, (2 * i + 5) * cxGrid / 2, 2 * cyGrid + 2, szBuffer,
                  wsprintfW(szBuffer, L"%X-", i));

         TextOutW(hdc, 3 * cxGrid / 2, (i + 3) * cyGrid + 2, szBuffer,
                  wsprintfW(szBuffer, L"-%X", i));
      }

      // characters
      for ( y = 0; y < 16; y++ )
         for ( x = 0; x < 16; x++ )
         {
            TextOutW(hdc, (2 * x + 5) * cxGrid / 2,
                     (y + 3) * cyGrid + 2, szBuffer,
                     wsprintfW(szBuffer, L"%c", 16 * x + y));
         }

      EndPaint(hwnd, &ps);
      return 0;

   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(hwnd, message, wParam, lParam);
}