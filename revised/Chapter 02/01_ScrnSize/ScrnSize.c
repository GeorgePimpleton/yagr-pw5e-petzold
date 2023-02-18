/*-----------------------------------------------------
   SCRNSIZE.C -- Displays screen size in a message box
                 (c) Charles Petzold, 1998
  -----------------------------------------------------*/

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>

int CDECL MessageBoxPrintf(_In_opt_z_ PCWSTR szCaption,
                           _In_z_     PCWSTR szFormat,
                                      ...)
{
   WCHAR   szBuffer[ 1024 ];
   va_list pArgList;

   // The va_start macro (defined in STDARG.H) is usually equivalent to:
   // pArgList = (char *) &szFormat + sizeof (szFormat) ;
   va_start(pArgList, szFormat);

   // The last argument to wvsprintf points to the arguments
   (void) _vsnwprintf_s(szBuffer, _countof(szBuffer) - 1, _TRUNCATE, szFormat, pArgList);

   // The va_end macro just zeroes out pArgList for no good reason
   va_end(pArgList);

   return MessageBoxW(NULL, szBuffer, szCaption, MB_OK);
}

int WINAPI wWinMain(_In_     HINSTANCE hInstance,
                    _In_opt_ HINSTANCE hPrevInstance,
                    _In_     PWSTR     pCmdLine,
                    _In_     int       nShowCmd)
{
   UNREFERENCED_PARAMETER(hInstance);
   UNREFERENCED_PARAMETER(hPrevInstance);
   UNREFERENCED_PARAMETER(pCmdLine);
   UNREFERENCED_PARAMETER(nShowCmd);

   int cxScreen;
   int cyScreen;

   cxScreen = GetSystemMetrics(SM_CXSCREEN);
   cyScreen = GetSystemMetrics(SM_CYSCREEN);

   MessageBoxPrintf(L"ScrnSize",
                    L"The screen is %i pixels wide by %i pixels high.",
                    cxScreen, cyScreen);

   return 0;
}