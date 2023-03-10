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
devcaps[] =
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

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(_In_     HINSTANCE instance,
                    _In_opt_ HINSTANCE prevInstance,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInstance);
   UNREFERENCED_PARAMETER(cmdLine);

   static PCWSTR appName = L"DevCaps1";
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

   HWND hwnd = CreateWindowW(appName, L"Device Capabilities",
                             WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             NULL, NULL, instance, NULL);

   ShowWindow(hwnd, showCmd);
   UpdateWindow(hwnd);

   MSG msg;

   while ( GetMessageW(&msg, NULL, 0, 0) )
   {
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
   }
   return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   static int  cxChar;
   static int  cxCaps;
   static int  cyChar;
   WCHAR       buffer[ 10 ];
   HDC         hdc;
   PAINTSTRUCT ps;
   TEXTMETRICW tm;

   switch ( message )
   {
   case WM_CREATE:
      hdc = GetDC(hwnd);

      GetTextMetricsW(hdc, &tm);

      cxChar = tm.tmAveCharWidth;
      cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2;
      cyChar = tm.tmHeight + tm.tmExternalLeading;

      ReleaseDC(hwnd, hdc);
      return 0;

   case WM_PAINT:
      hdc = BeginPaint(hwnd, &ps);

      for ( unsigned i = 0; i < _countof(devcaps); i++ )
      {
         TextOutW(hdc, 1, cyChar * i,
                  devcaps[ i ].label,
                  lstrlenW(devcaps[ i ].label));

         TextOutW(hdc, 14 * cxCaps + 1, cyChar * i,
                  devcaps[ i ].desc,
                  lstrlenW(devcaps[ i ].desc));

         SetTextAlign(hdc, TA_RIGHT | TA_TOP);

         TextOutW(hdc, 14 * cxCaps + 35 * cxChar + 1, cyChar * i, buffer,
                  wsprintfW(buffer, L"%5d",
                            GetDeviceCaps(hdc, devcaps[ i ].index)));

         SetTextAlign(hdc, TA_LEFT | TA_TOP);
      }

      EndPaint(hwnd, &ps);
      return 0;

   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   }

   return DefWindowProcW(hwnd, message, wParam, lParam);
}