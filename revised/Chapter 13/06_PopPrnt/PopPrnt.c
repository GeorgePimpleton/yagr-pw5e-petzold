/*----------------------------------------------
   POPPRNT.C -- Popup Editor Printing Functions
  ----------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <commdlg.h>
#include <malloc.h>
#include "Resource.h"

BOOL userAbort;
HWND dlgPrint;

INT_PTR CALLBACK PrintDlgProc(HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
   UNREFERENCED_PARAMETER(wParam);
   UNREFERENCED_PARAMETER(lParam);

   switch (msg)
   {
   case WM_INITDIALOG:
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

BOOL CALLBACK AbortProc(HDC printerDC, int code)
{
   MSG msg;

   while (!userAbort && PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
   {
      if (!dlgPrint || !IsDialogMessageW(dlgPrint, &msg))
      {
         TranslateMessage(&msg);
         DispatchMessageW(&msg);
      }
   }
   return !userAbort;
}

BOOL PopPrntPrintFile(HINSTANCE inst, HWND wnd, HWND wndEdit, PWSTR titleName)
{
   static DOCINFOW  di = { sizeof(DOCINFOW) };
   static PRINTDLGW pd;
   BOOL             success;
   int              yChar;
   int              charsPerLine;
   int              linesPerPage;
   int              totalLines;
   int              totalPages;
   int              page;
   int              line;
   int              lineNum;
   PWSTR            strBuffer;
   TCHAR            jobName[64 + MAX_PATH];
   TEXTMETRIC       tm;
   WORD             colCopy;
   WORD             noiColCopy;

   // Invoke Print common dialog box
   pd.lStructSize         = sizeof(PRINTDLGW);
   pd.hwndOwner           = wnd;
   pd.hDevMode            = NULL;
   pd.hDevNames           = NULL;
   pd.hDC                 = NULL;
   pd.Flags               = PD_ALLPAGES | PD_COLLATE | PD_RETURNDC | PD_NOSELECTION;
   pd.nFromPage           = 0;
   pd.nToPage             = 0;
   pd.nMinPage            = 0;
   pd.nMaxPage            = 0;
   pd.nCopies             = 1;
   pd.hInstance           = NULL;
   pd.lCustData           = 0L;
   pd.lpfnPrintHook       = NULL;
   pd.lpfnSetupHook       = NULL;
   pd.lpPrintTemplateName = NULL;
   pd.lpSetupTemplateName = NULL;
   pd.hPrintTemplate      = NULL;
   pd.hSetupTemplate      = NULL;

   if ( !PrintDlg(&pd) )
   {
      return TRUE;
   }

   if ( 0 == (totalLines = SendMessage(wndEdit, EM_GETLINECOUNT, 0, 0)) )
   {
      return TRUE;
   }

   // Calculate necessary metrics for file
   GetTextMetricsW(pd.hDC, &tm);
   yChar = tm.tmHeight + tm.tmExternalLeading;

   charsPerLine = GetDeviceCaps(pd.hDC, HORZRES) / tm.tmAveCharWidth;
   linesPerPage = GetDeviceCaps(pd.hDC, VERTRES) / yChar;
   totalPages   = (totalLines + linesPerPage - 1) / linesPerPage;

   // Allocate a buffer for each line of text
   strBuffer = (PWSTR) malloc(sizeof(WCHAR) * (charsPerLine + 1));

   // Display the printing dialog box
   EnableWindow(wnd, FALSE);

   success   = TRUE;
   userAbort = FALSE;

   dlgPrint = CreateDialogParamW(inst, L"PrintDlgBox", wnd, PrintDlgProc, 0L);

   SetDlgItemTextW(dlgPrint, IDC_FILENAME, titleName);
   SetAbortProc(pd.hDC, AbortProc);

   // Start the document
   GetWindowTextW(wnd, jobName, sizeof(jobName));
   di.lpszDocName = jobName;

   if (StartDocW(pd.hDC, &di) > 0)
   {
      // Collation requires this loop and iNoiColCopy

      for ( colCopy = 0;
           colCopy < ((WORD) pd.Flags & PD_COLLATE ? pd.nCopies : 1);
           colCopy++ )
      {
         for (page = 0; page < totalPages; page++)
         {
            for ( noiColCopy = 0;
                 noiColCopy < (pd.Flags & PD_COLLATE ? 1 : pd.nCopies);
                 noiColCopy++ )
            {
               // Start the page
               if (StartPage(pd.hDC) < 0)
               {
                  success = FALSE;
                  break;
               }

               // For each page, print the lines
               for (line = 0; line < linesPerPage; line++)
               {
                  lineNum = linesPerPage * page + line;

                  if ( lineNum > totalLines )
                  {
                     break;
                  }

                  *(int*)strBuffer = charsPerLine;

                  TextOutW(pd.hDC, 0, yChar * line, strBuffer,
                           (int) SendMessageW(wndEdit, EM_GETLINE,
                                              (WPARAM) lineNum, (LPARAM) strBuffer));
               }

               if (EndPage(pd.hDC) < 0)
               {
                  success = FALSE;
                  break;
               }

               if ( userAbort )
               {
                  break;
               }
            }

            if ( !success || userAbort )
            {
               break;
            }
         }

         if ( !success || userAbort )
         {
            break;
         }
      }
   }
   else
   {
      success = FALSE;
   }

   if ( success )
   {
      EndDoc(pd.hDC);
   }

   if (!userAbort)
   {
      EnableWindow(wnd, TRUE);
      DestroyWindow(dlgPrint);
   }

   free(strBuffer);
   DeleteDC(pd.hDC);

   return success && !userAbort;
}