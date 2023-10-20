/*---------------------------------------------------------
   DEVCAPS1.C -- Device Capabilities Display Program No. 1
                 (c) Charles Petzold, 1998
  ---------------------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>

struct
{
   int    index;
   PCWSTR label;
   PCWSTR desc;
}
devcaps[ ] =
{
   {HORZSIZE,      L"HORZSIZE",     L"Width in millimeters:"},
   {VERTSIZE,      L"VERTSIZE",     L"Height in millimeters:"},
   {HORZRES,       L"HORZRES",      L"Width in pixels:"},
   {VERTRES,       L"VERTRES",      L"Height in raster lines:"},
   {BITSPIXEL,     L"BITSPIXEL",    L"Color bits per pixel:"},
   {PLANES,        L"PLANES",       L"Number of color planes:"},
   {NUMBRUSHES,    L"NUMBRUSHES",   L"Number of device brushes:"},
   {NUMPENS,       L"NUMPENS",      L"Number of device pens:"},
   {NUMMARKERS,    L"NUMMARKERS",   L"Number of device markers:"},
   {NUMFONTS,      L"NUMFONTS",     L"Number of device fonts:"},
   {NUMCOLORS,     L"NUMCOLORS",    L"Number of device colors:"},
   {PDEVICESIZE,   L"PDEVICESIZE",  L"Size of device structure:"},
   {ASPECTX,       L"ASPECTX",      L"Relative width of pixel:"},
   {ASPECTY,       L"ASPECTY",      L"Relative height of pixel:"},
   {ASPECTXY,      L"ASPECTXY",     L"Relative diagonal of pixel:"},
   {LOGPIXELSX,    L"LOGPIXELSX",   L"Horizontal dots per inch:"},
   {LOGPIXELSY,    L"LOGPIXELSY",   L"Vertical dots per inch:"},
   {SIZEPALETTE,   L"SIZEPALETTE",  L"Number of palette entries:"},
   {NUMRESERVED,   L"NUMRESERVED",  L"Reserved palette entries:"},
   {COLORRES,      L"COLORRES",     L"Actual color resolution:"}
};

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(_In_     HINSTANCE inst,
                    _In_opt_ HINSTANCE prevInst,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInst);
   UNREFERENCED_PARAMETER(cmdLine);

   static PCWSTR appName = L"DevCaps1";
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
      MessageBoxW(NULL, L"This program requires Windows NT!",
                  appName, MB_ICONERROR);
      return 0;
   }

   HWND wnd = CreateWindowW(appName, L"Device Capabilities",
                            WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            NULL, NULL, inst, NULL);

   ShowWindow(wnd, showCmd);
   UpdateWindow(wnd);

   MSG msg;

   while ( GetMessageW(&msg, NULL, 0, 0) )
   {
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
   }
   return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static int  xChar;
   static int  xCaps;
   static int  yChar;
   WCHAR       buffer[ 10 ] = { L"" };
   HDC         dc;
   PAINTSTRUCT ps;
   TEXTMETRICW tm;

   switch ( msg )
   {
   case WM_CREATE:
      dc = GetDC(wnd);

      GetTextMetricsW(dc, &tm);

      xChar = tm.tmAveCharWidth;
      xCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * xChar / 2;
      yChar = tm.tmHeight + tm.tmExternalLeading;

      ReleaseDC(wnd, dc);
      return 0;

   case WM_PAINT:
      dc = BeginPaint(wnd, &ps);

      for ( unsigned i = 0; i < _countof(devcaps); i++ )
      {
         TextOutW(dc, 1, yChar * i,
                  devcaps[ i ].label,
                  lstrlenW(devcaps[ i ].label));

         TextOutW(dc, 14 * xCaps + 1, yChar * i,
                  devcaps[ i ].desc,
                  lstrlenW(devcaps[ i ].desc));

         SetTextAlign(dc, TA_RIGHT | TA_TOP);

         TextOutW(dc, 14 * xCaps + 35 * xChar + 1, yChar * i, buffer,
                  wsprintfW(buffer, L"%5d",
                            GetDeviceCaps(dc, devcaps[ i ].index)));

         SetTextAlign(dc, TA_LEFT | TA_TOP);
      }

      EndPaint(wnd, &ps);
      return 0;

   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(wnd, msg, wParam, lParam);
}