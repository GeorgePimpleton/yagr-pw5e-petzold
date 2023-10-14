/*---------------------------------------------
   FORMFEED.C -- Advances printer to next page
                 (c) Charles Petzold, 1998
  ---------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

HDC GetPrinterDC(void);

int WINAPI wWinMain(_In_     HINSTANCE inst,
                    _In_opt_ HINSTANCE orevInst,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(inst);
   UNREFERENCED_PARAMETER(orevInst);
   UNREFERENCED_PARAMETER(cmdLine);
   UNREFERENCED_PARAMETER(showCmd);

   static DOCINFO di       = { .cbSize = sizeof(DOCINFO), .lpszDocName = L"FormFeed" };
   HDC            hdcPrint = GetPrinterDC( );

   if ( hdcPrint != NULL )
   {
      if ( StartDocW(hdcPrint, &di) > 0 )
      {
         if ( StartPage(hdcPrint) > 0 && EndPage(hdcPrint) > 0 )
         {
            EndDoc(hdcPrint);
         }
      }

      DeleteDC(hdcPrint);
   }
   return 0;
}
