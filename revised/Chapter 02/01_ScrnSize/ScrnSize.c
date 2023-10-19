/*-----------------------------------------------------
   SCRNSIZE.C -- Displays screen size in a message box
                 (c) Charles Petzold, 1998
  -----------------------------------------------------*/

#define WIN32_MEAN_AND_LEAN

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>

int CDECL MessageBoxPrintf(_In_opt_z_ PCWSTR caption,
                           _In_z_     PCWSTR format,
                           ...)
{
   WCHAR   buffer[ 1024 ];
   va_list argList;

   // The va_start macro (defined in STDARG.H) is usually equivalent to:
   // pArgList = (char *) &szFormat + sizeof (szFormat) ;
   va_start(argList, format);

   // The last argument to wvsprintf points to the arguments
   (void) _vsnwprintf_s(buffer, _countof(buffer) - 1, _TRUNCATE, format, argList);

   // The va_end macro just zeroes out pArgList for no good reason
   va_end(argList);

   return MessageBoxW(NULL, buffer, caption, MB_OK);
}

int WINAPI wWinMain(_In_     HINSTANCE inst,
                    _In_opt_ HINSTANCE prevInst,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(inst);
   UNREFERENCED_PARAMETER(prevInst);
   UNREFERENCED_PARAMETER(cmdLine);
   UNREFERENCED_PARAMETER(showCmd);

   int cxScreen;
   int cyScreen;

   cxScreen = GetSystemMetrics(SM_CXSCREEN);
   cyScreen = GetSystemMetrics(SM_CYSCREEN);

   MessageBoxPrintf(L"ScrnSize",
                    L"The screen is %i pixels wide by %i pixels high.",
                    cxScreen, cyScreen);

   return 0;
}