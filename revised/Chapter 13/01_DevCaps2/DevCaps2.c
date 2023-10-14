/*------------------------------------------------------------------
   DEVCAPS2.C -- Displays Device Capability Information (Version 2)
                 (c) Charles Petzold, 1998
------------------------------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winspool.h>
#include <stdlib.h>
#include "Resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DoBasicInfo(HDC, HDC, int, int);
void DoOtherInfo(HDC, HDC, int, int);
void DoBitCodedCaps(HDC, HDC, int, int, int);

typedef struct
{
   int    mask;
   PCWSTR desc;
}
BITS;

#define IDM_DEVMODE      1000

int WINAPI wWinMain(_In_     HINSTANCE inst,
                    _In_opt_ HINSTANCE prevInst,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInst);
   UNREFERENCED_PARAMETER(cmdLine);

   static PCWSTR appName = L"DevCaps2";
   HWND          wnd;
   MSG           msg;
   WNDCLASSW     wc;

   wc.style = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc = WndProc;
   wc.cbClsExtra = 0;
   wc.cbWndExtra = 0;
   wc.hInstance = inst;
   wc.hIcon = (HICON) LoadImageW(inst, appName, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hCursor = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
   wc.lpszMenuName = appName;
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

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   static WCHAR           device[ 32 ];
   static WCHAR           windowText[ 64 ];
   static int             xChar;
   static int             yChar;
   static int             currentDevice = IDM_SCREEN;
   static int             currentInfo   = IDM_BASIC;
   static DWORD           needed;
   static DWORD           returned;
   static PRINTER_INFO_4* pinfo4;
   DWORD                  i;
   HDC                    dc;
   HDC                    dcInfo;
   HMENU                  menu;
   HANDLE                 print;
   PAINTSTRUCT            ps;
   TEXTMETRIC             tm;

   switch ( message )
   {
   case WM_CREATE:
      dc = GetDC(hwnd);
      SelectObject(dc, GetStockObject(SYSTEM_FIXED_FONT));
      GetTextMetricsW(dc, &tm);
      xChar = tm.tmAveCharWidth;
      yChar = tm.tmHeight + tm.tmExternalLeading;
      ReleaseDC(hwnd, dc);
      // fall through

   case WM_SETTINGCHANGE:
      menu = GetSubMenu(GetMenu(hwnd), 0);

      while ( GetMenuItemCount(menu) > 1 )
      {
         DeleteMenu(menu, 1, MF_BYPOSITION);
      }

      // Get a list of all local and remote printers
      //
      // First, find out how large an array we need; this
      //   call will fail, leaving the required size in dwNeeded
      //
      // Next, allocate space for the info array and fill it
      //
      // Put the printer names on the menu.

      EnumPrintersW(PRINTER_ENUM_LOCAL, NULL, 4, NULL,
                    0, &needed, &returned);

      pinfo4 = (PRINTER_INFO_4*) malloc(needed);

      EnumPrintersW(PRINTER_ENUM_LOCAL, NULL, 4, (PBYTE) pinfo4,
                    needed, &needed, &returned);

      for ( i = 0; i < returned; i++ )
      {
         AppendMenuW(menu, (i + 1) % 16 ? 0 : MF_MENUBARBREAK, i + 1,
                     pinfo4[ i ].pPrinterName);
      }
      free(pinfo4);

      AppendMenuW(menu, MF_SEPARATOR, 0, NULL);
      AppendMenuW(menu, 0, IDM_DEVMODE, L"Properties");

      wParam = IDM_SCREEN;
      // fall through

   case WM_COMMAND:
      menu = GetMenu(hwnd);

      if ( LOWORD(wParam) == IDM_SCREEN ||         // IDM_SCREEN & Printers
           LOWORD(wParam) < IDM_DEVMODE )
      {
         CheckMenuItem(menu, currentDevice, MF_UNCHECKED);
         currentDevice = LOWORD(wParam);
         CheckMenuItem(menu, currentDevice, MF_CHECKED);
      }
      else if ( LOWORD(wParam) == IDM_DEVMODE )     // Properties selection
      {
         GetMenuStringW(menu, currentDevice, device,
                        _countof(device), MF_BYCOMMAND);

         if ( OpenPrinterW(device, &print, NULL) )
         {
            PrinterProperties(hwnd, print);
            ClosePrinter(print);
         }
      }
      else                               // info menu items
      {
         CheckMenuItem(menu, currentInfo, MF_UNCHECKED);
         currentInfo = LOWORD(wParam);
         CheckMenuItem(menu, currentInfo, MF_CHECKED);
      }
      InvalidateRect(hwnd, NULL, TRUE);
      return 0;

   case WM_INITMENUPOPUP:
      if ( lParam == 0 )
      {
         EnableMenuItem(GetMenu(hwnd), IDM_DEVMODE,
                        currentDevice == IDM_SCREEN ? MF_GRAYED : MF_ENABLED);
      }
      return 0;

   case WM_PAINT:
      lstrcpyW(windowText, L"Device Capabilities: ");

      if ( currentDevice == IDM_SCREEN )
      {
         lstrcpyW(device, L"DISPLAY");
         dcInfo = CreateICW(device, NULL, NULL, NULL);
      }
      else
      {
         menu = GetMenu(hwnd);
         GetMenuStringW(menu, currentDevice, device,
                        sizeof(device), MF_BYCOMMAND);
         dcInfo = CreateICW(NULL, device, NULL, NULL);
      }

      lstrcat(windowText, device);
      SetWindowText(hwnd, windowText);

      dc = BeginPaint(hwnd, &ps);
      SelectObject(dc, GetStockObject(SYSTEM_FIXED_FONT));

      if ( dcInfo )
      {
         switch ( currentInfo )
         {
         case IDM_BASIC:
            DoBasicInfo(dc, dcInfo, xChar, yChar);
            break;

         case IDM_OTHER:
            DoOtherInfo(dc, dcInfo, xChar, yChar);
            break;

         case IDM_CURVE:
         case IDM_LINE:
         case IDM_POLY:
         case IDM_TEXT:
            DoBitCodedCaps(dc, dcInfo, xChar, yChar,
                           currentInfo - IDM_CURVE);
            break;
         }
         DeleteDC(dcInfo);
      }

      EndPaint(hwnd, &ps);
      return 0;

   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   }
   return DefWindowProcW(hwnd, message, wParam, lParam);
}

void DoBasicInfo(HDC hdc, HDC hdcInfo, int cxChar, int cyChar)
{
   static struct
   {
      int    index;
      PCWSTR desc;
   }
   info[ ] = { {HORZSIZE,        L"HORZSIZE        Width in millimeters:"},
               {VERTSIZE,        L"VERTSIZE        Height in millimeters:"},
               {HORZRES,         L"HORZRES         Width in pixels:"},
               {VERTRES,         L"VERTRES         Height in raster lines:"},
               {BITSPIXEL,       L"BITSPIXEL       Color bits per pixel:"},
               {PLANES,          L"PLANES          Number of color planes:"},
               {NUMBRUSHES,      L"NUMBRUSHES      Number of device brushes:"},
               {NUMPENS,         L"NUMPENS         Number of device pens:"},
               {NUMMARKERS,      L"NUMMARKERS      Number of device markers:"},
               {NUMFONTS,        L"NUMFONTS        Number of device fonts:"},
               {NUMCOLORS,       L"NUMCOLORS       Number of device colors:"},
               {PDEVICESIZE,     L"PDEVICESIZE     Size of device structure:"},
               {ASPECTX,         L"ASPECTX         Relative width of pixel:"},
               {ASPECTY,         L"ASPECTY         Relative height of pixel:"},
               {ASPECTXY,        L"ASPECTXY        Relative diagonal of pixel:"},
               {LOGPIXELSX,      L"LOGPIXELSX      Horizontal dots per inch:"},
               {LOGPIXELSY,      L"LOGPIXELSY      Vertical dots per inch:"},
               {SIZEPALETTE,     L"SIZEPALETTE     Number of palette entries:"},
               {NUMRESERVED,     L"NUMRESERVED     Reserved palette entries:"},
               {COLORRES,        L"COLORRES        Actual color resolution:"},
               {PHYSICALWIDTH,   L"PHYSICALWIDTH   Printer page pixel width:"},
               {PHYSICALHEIGHT,  L"PHYSICALHEIGHT  Printer page pixel height:"},
               {PHYSICALOFFSETX, L"PHYSICALOFFSETX Printer page x offset:"},
               {PHYSICALOFFSETY, L"PHYSICALOFFSETY Printer page y offset:"} };
   WCHAR buffer[ 80 ];

   for ( unsigned i = 0; i < _countof(info); i++ )
   {
      TextOutW(hdc, cxChar, (i + 1)* cyChar, buffer,
               wsprintfW(buffer, L"%-45s%8d", info[ i ].desc,
                         GetDeviceCaps(hdcInfo, info[ i ].index)));
   }
}

void DoOtherInfo(HDC dc, HDC dcInfo, int xChar, int yChar)
{
   static BITS clip[ ] =
   {
      {CP_RECTANGLE,    L"CP_RECTANGLE    Can Clip To Rectangle:"}
   };

   static BITS raster[ ] =
   {
      {RC_BITBLT,       L"RC_BITBLT       Capable of simple BitBlt:"},
      {RC_BANDING,      L"RC_BANDING      Requires banding support:"},
      {RC_SCALING,      L"RC_SCALING      Requires scaling support:"},
      {RC_BITMAP64,     L"RC_BITMAP64     Supports bitmaps >64K:"},
      {RC_GDI20_OUTPUT, L"RC_GDI20_OUTPUT Has 2.0 output calls:"},
      {RC_DI_BITMAP,    L"RC_DI_BITMAP    Supports DIB to memory:"},
      {RC_PALETTE,      L"RC_PALETTE      Supports a palette:"},
      {RC_DIBTODEV,     L"RC_DIBTODEV     Supports bitmap conversion:"},
      {RC_BIGFONT,      L"RC_BIGFONT      Supports fonts >64K:"},
      {RC_STRETCHBLT,   L"RC_STRETCHBLT   Supports StretchBlt:"},
      {RC_FLOODFILL,    L"RC_FLOODFILL    Supports FloodFill:"},
      {RC_STRETCHDIB,   L"RC_STRETCHDIB   Supports StretchDIBits:"}
   };

   static PCWSTR tech[ ] =
   {
      L"DT_PLOTTER (Vector plotter)",
      L"DT_RASDISPLAY (Raster display)",
      L"DT_RASPRINTER (Raster printer)",
      L"DT_RASCAMERA (Raster camera)",
      L"DT_CHARSTREAM (Character stream)",
      L"DT_METAFILE (Metafile)",
      L"DT_DISPFILE (Display file)"
   };
   WCHAR          buffer[ 80 ];

   TextOutW(dc, xChar, yChar, buffer,
            wsprintfW(buffer, L"%-24s%04XH", L"DRIVERVERSION:",
                      GetDeviceCaps(dcInfo, DRIVERVERSION)));

   TextOutW(dc, xChar, 2 * yChar, buffer,
            wsprintfW(buffer, L"%-24s%-40s", L"TECHNOLOGY:",
                      tech[ GetDeviceCaps(dcInfo, TECHNOLOGY) ]));

   TextOutW(dc, xChar, 4 * yChar, buffer,
            wsprintfW(buffer, L"CLIPCAPS (Clipping capabilities)"));

   for ( unsigned i = 0; i < _countof(clip); i++ )
   {
      TextOutW(dc, 9 * xChar, (i + 6) * yChar, buffer,
               wsprintfW(buffer, L"%-45s %3s", clip[ i ].desc,
                         GetDeviceCaps(dcInfo, CLIPCAPS) & clip[ i ].mask ?
                         L"Yes" : L"No"));
   }

   TextOutW(dc, xChar, 8 * yChar, buffer,
            wsprintfW(buffer, L"RASTERCAPS (Raster capabilities)"));

   for ( unsigned i = 0; i < _countof(raster); i++ )
   {
      TextOutW(dc, 9 * xChar, (i + 10) * yChar, buffer,
               wsprintfW(buffer, L"%-45s %3s", raster[ i ].desc,
                         GetDeviceCaps(dcInfo, RASTERCAPS) & raster[ i ].mask ?
                         L"Yes" : L"No"));
   }
}

void DoBitCodedCaps(HDC dc, HDC dcInfo, int xChar, int yChar, int type)
{
   static BITS curves[ ] =
   {
      { CC_CIRCLES,    L"CC_CIRCLES    Can do circles:"                 },
      { CC_PIE,        L"CC_PIE        Can do pie wedges:"              },
      { CC_CHORD,      L"CC_CHORD      Can do chord arcs:"              },
      { CC_ELLIPSES,   L"CC_ELLIPSES   Can do ellipses:"                },
      { CC_WIDE,       L"CC_WIDE       Can do wide borders:"            },
      { CC_STYLED,     L"CC_STYLED     Can do styled borders:"          },
      { CC_WIDESTYLED, L"CC_WIDESTYLED Can do wide and styled borders:" },
      { CC_INTERIORS,  L"CC_INTERIORS  Can do interiors:"               }
   };

   static BITS lines[ ] =
   {
      { LC_POLYLINE,   L"LC_POLYLINE   Can do polyline:"              },
      { LC_MARKER,     L"LC_MARKER     Can do markers:"               },
      { LC_POLYMARKER, L"LC_POLYMARKER Can do polymarkers"            },
      { LC_WIDE,       L"LC_WIDE       Can do wide lines:"            },
      { LC_STYLED,     L"LC_STYLED     Can do styled lines:"          },
      { LC_WIDESTYLED, L"LC_WIDESTYLED Can do wide and styled lines:" },
      { LC_INTERIORS,  L"LC_INTERIORS  Can do interiors:"             }
   };

   static BITS poly[ ] =
   {
      { PC_POLYGON,     L"PC_POLYGON     Can do alternate fill polygon:"      },
      { PC_RECTANGLE,   L"PC_RECTANGLE   Can do rectangle:"                   },
      { PC_WINDPOLYGON, L"PC_WINDPOLYGON Can do winding number fill polygon:" },
      { PC_SCANLINE,    L"PC_SCANLINE    Can do scanlines:"                   },
      { PC_WIDE,        L"PC_WIDE        Can do wide borders:"                },
      { PC_STYLED,      L"PC_STYLED      Can do styled borders:"              },
      { PC_WIDESTYLED,  L"PC_WIDESTYLED  Can do wide and styled borders:"     },
      { PC_INTERIORS,   L"PC_INTERIORS   Can do interiors:"                   }
   };

   static BITS text[ ] =
   {
      { TC_OP_CHARACTER, L"TC_OP_CHARACTER Can do character output precision:"      },
      { TC_OP_STROKE,    L"TC_OP_STROKE    Can do stroke output precision:"         },
      { TC_CP_STROKE,    L"TC_CP_STROKE    Can do stroke clip precision:"           },
      { TC_CR_90,        L"TC_CP_90        Can do 90 degree character rotation:"    },
      { TC_CR_ANY,       L"TC_CR_ANY       Can do any character rotation:"          },
      { TC_SF_X_YINDEP,  L"TC_SF_X_YINDEP  Can do scaling independent of X and Y:"  },
      { TC_SA_DOUBLE,    L"TC_SA_DOUBLE    Can do doubled character for scaling:"   },
      { TC_SA_INTEGER,   L"TC_SA_INTEGER   Can do integer multiples for scaling:"   },
      { TC_SA_CONTIN,    L"TC_SA_CONTIN    Can do any multiples for exact scaling:" },
      { TC_EA_DOUBLE,    L"TC_EA_DOUBLE    Can do double weight characters:"        },
      { TC_IA_ABLE,      L"TC_IA_ABLE      Can do italicizing:"                     },
      { TC_UA_ABLE,      L"TC_UA_ABLE      Can do underlining:"                     },
      { TC_SO_ABLE,      L"TC_SO_ABLE      Can do strikeouts:"                      },
      { TC_RA_ABLE,      L"TC_RA_ABLE      Can do raster fonts:"                    },
      { TC_VA_ABLE,      L"TC_VA_ABLE      Can do vector fonts:"                    }
   };

   static struct
   {
      int    index;
      PCWSTR title;
      BITS   (*pbits)[ ];
      int    size;
   }
   bitinfo[ ] =
   {
      { CURVECAPS,     L"CURVCAPS (Curve Capabilities)",          (BITS(*)[ ]) curves, _countof(curves) },
      { LINECAPS,      L"LINECAPS (Line Capabilities)",           (BITS(*)[ ]) lines,  _countof(lines)  },
      { POLYGONALCAPS, L"POLYGONALCAPS (Polygonal Capabilities)", (BITS(*)[ ]) poly,   _countof(poly)   },
      { TEXTCAPS,      L"TEXTCAPS (Text Capabilities)",           (BITS(*)[ ]) text,   _countof(text)   }
   };

   static WCHAR buffer[ 80 ];
   BITS         (*pbits)[ ] = bitinfo[ type ].pbits;
   int          i;
   int          iDevCaps    = GetDeviceCaps(dcInfo, bitinfo[ type ].index);

   TextOutW(dc, xChar, yChar, bitinfo[ type ].title,
            lstrlenW(bitinfo[ type ].title));

   for ( i = 0; i < bitinfo[ type ].size; i++ )
   {
      TextOutW(dc, xChar, (i + 3) * yChar, buffer,
              wsprintfW(buffer, L"%-55s %3s", (*pbits)[ i ].desc,
                       iDevCaps & (*pbits)[ i ].mask ? L"Yes" : L"No"));
   }
}