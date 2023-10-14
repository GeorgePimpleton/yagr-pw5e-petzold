/*---------------------------------------
   PRINT1.C -- Bare Bones Printing
               (c) Charles Petzold, 1998
  ---------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "Print.h"

HINSTANCE xInst;
PCWSTR    xAppName = L"Print1";
PCWSTR    xCaption = L"Print Program 1";

BOOL PrintMyPage(HWND hwnd)
{
   static DOCINFOW di      = { sizeof(DOCINFOW), L"Print1: Printing" };
   BOOL            success = TRUE;
   HDC             dcPrn;
   int             xPage;
   int             yPage;

   if ( NULL == (dcPrn = GetPrinterDC( )) )
   {
      return FALSE;
   }

   xPage = GetDeviceCaps(dcPrn, HORZRES);
   yPage = GetDeviceCaps(dcPrn, VERTRES);

   if ( StartDoc(dcPrn, &di) > 0 )
   {
      if ( StartPage(dcPrn) > 0 )
      {
         PageGDICalls(dcPrn, xPage, yPage);

         if ( EndPage(dcPrn) > 0 )
         {
            EndDoc(dcPrn);
         }
         else
         {
            success = FALSE;
         }
      }
   }
   else
   {
      success = FALSE;
   }

   DeleteDC(dcPrn);
   return success;
}