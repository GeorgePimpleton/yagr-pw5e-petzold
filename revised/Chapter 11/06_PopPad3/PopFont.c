/*------------------------------------------
   POPFONT.C -- Popup Editor Font Functions
  ------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>

static LOGFONTW logfont;
static HFONT    font;

BOOL PopFontChooseFont(HWND wnd)
{
   CHOOSEFONTW cf;

   cf.lStructSize    = sizeof(CHOOSEFONTW);
   cf.hwndOwner      = wnd;
   cf.hDC            = NULL;
   cf.lpLogFont      = &logfont;
   cf.iPointSize     = 0;
   cf.Flags          = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS | CF_EFFECTS;
   cf.rgbColors      = 0;
   cf.lCustData      = 0;
   cf.lpfnHook       = NULL;
   cf.lpTemplateName = NULL;
   cf.hInstance      = NULL;
   cf.lpszStyle      = NULL;
   cf.nFontType      = 0;               // Returned from ChooseFont
   cf.nSizeMin       = 0;
   cf.nSizeMax       = 0;

   return ChooseFontW(&cf);
}

void PopFontInitialize(HWND wndEdit)
{
   GetObjectW(GetStockObject(SYSTEM_FONT), sizeof(LOGFONTW),
              (PWSTR) &logfont);

   font = CreateFontIndirectW(&logfont);
   SendMessageW(wndEdit, WM_SETFONT, (WPARAM) font, 0);
}

void PopFontSetFont(HWND wndEdit)
{
   HFONT fontNew;
   RECT  rect;

   fontNew = CreateFontIndirectW(&logfont);
   SendMessageW(wndEdit, WM_SETFONT, (WPARAM) fontNew, 0);
   DeleteObject(font);
   font = fontNew;
   GetClientRect(wndEdit, &rect);
   InvalidateRect(wndEdit, &rect, TRUE);
}

void PopFontDeinitialize(void)
{
   DeleteObject(font);
}
