/*----------------------------------------------
   COLORS3.C -- Version using Common Dialog Box
                (c) Charles Petzold, 1998
  ----------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <commdlg.h>

int WINAPI wWinMain(_In_     HINSTANCE inst,
                    _In_opt_ HINSTANCE prevInst,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(inst);
   UNREFERENCED_PARAMETER(prevInst);
   UNREFERENCED_PARAMETER(cmdLine);
   UNREFERENCED_PARAMETER(showCmd);

   static CHOOSECOLORW cc;
   static COLORREF     custColors[16];

   cc.lStructSize    = sizeof(CHOOSECOLORW);
   cc.hwndOwner      = NULL;
   cc.hInstance      = NULL;
   cc.rgbResult      = RGB(0x80, 0x80, 0x80);
   cc.lpCustColors   = custColors;
   cc.Flags          = CC_RGBINIT | CC_FULLOPEN;
   cc.lCustData      = 0;
   cc.lpfnHook       = NULL;
   cc.lpTemplateName = NULL;

   return ChooseColorW(&cc);
}