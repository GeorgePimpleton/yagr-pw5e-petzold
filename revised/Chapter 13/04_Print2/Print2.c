/*-------------------------------------------
   PRINT2.C -- Printing with Abort Procedure
               (c) Charles Petzold, 1998
  -------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "Print.h"

HINSTANCE xInst;
PCWSTR    xAppName = L"Print2";
PCWSTR    xCaption = L"Print Program 2 (Abort Procedure)";

BOOL CALLBACK AbortProc(HDC dcPrn, int code)
{
   MSG msg;

   while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
   {
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
   }
   return TRUE;
}

BOOL PrintMyPage(HWND wnd)
{
   static DOCINFOW di      = { sizeof(DOCINFOW), L"Print2: Printing" };
   BOOL            success = TRUE;
   HDC             dcPrn;
   short           xPage;
   int             yPage;

   if ( NULL == (dcPrn = GetPrinterDC( )) )
   {
      return FALSE;
   }

   xPage = GetDeviceCaps(dcPrn, HORZRES);
   yPage = GetDeviceCaps(dcPrn, VERTRES);

   EnableWindow(wnd, FALSE);

   SetAbortProc(dcPrn, AbortProc);

   if (StartDocW(dcPrn, &di) > 0)
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

   EnableWindow(wnd, TRUE);
   DeleteDC(dcPrn);
   return success;
}