/*---------------------------------------
   PRINT3.C -- Printing with Dialog Box
            (c) Charles Petzold, 1998
  ---------------------------------------*/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "Print.h"

HINSTANCE xInst;
PCWSTR    xAppName = L"Print3";
PCWSTR    xCaption = L"Print Program 3 (Dialog Box)";

BOOL userAbort;
HWND dlgPrint;

INT_PTR CALLBACK PrintDlgProc(HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch ( msg )
   {
   case WM_INITDIALOG:
      SetWindowTextW(dlg, xAppName);
      EnableMenuItem(GetSystemMenu(dlg, FALSE), SC_CLOSE, MF_GRAYED);
      return TRUE;

   case WM_COMMAND:
      userAbort = TRUE;
      EnableWindow(GetParent(dlg), TRUE);
      DestroyWindow(dlg);
      dlgPrint = NULL;
      return TRUE;
   }
   return FALSE;
}

BOOL CALLBACK AbortProc(HDC dcPrn, int code)
{
   MSG msg;

   while ( !userAbort && PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE) )
   {
      if ( !dlgPrint || !IsDialogMessageW(dlgPrint, &msg) )
      {
         TranslateMessage(&msg);
         DispatchMessageW(&msg);
      }
   }
   return !userAbort;
}

BOOL PrintMyPage(HWND wnd)
{
   static DOCINFOW di      = { sizeof(DOCINFOW), L"Print3: Printing" };
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

   EnableWindow(wnd, FALSE);

   userAbort = FALSE;
   dlgPrint = CreateDialogW(xInst, L"PrintDlgBox", wnd, PrintDlgProc);

   SetAbortProc(dcPrn, AbortProc);

   if ( StartDocW(dcPrn, &di) > 0 )
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

   if ( !userAbort )
   {
      EnableWindow(wnd, TRUE);
      DestroyWindow(dlgPrint);
   }

   DeleteDC(dcPrn);

   return success && !userAbort;
}