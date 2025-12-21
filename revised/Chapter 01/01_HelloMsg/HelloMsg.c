/*--------------------------------------------------------------
   HelloMsg.c -- Displays "Hello, Windows 98!" in a message box
                 (c) Charles Petzold, 1998
  --------------------------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

int WINAPI wWinMain( _In_     HINSTANCE inst,
                     _In_opt_ HINSTANCE prevInst,
                     _In_     PWSTR     cmdLine,
                     _In_     int       showCmd )
{
   UNREFERENCED_PARAMETER( inst );
   UNREFERENCED_PARAMETER( prevInst );
   UNREFERENCED_PARAMETER( cmdLine );
   UNREFERENCED_PARAMETER( showCmd );

   MessageBoxW( NULL, L"Hello, Windows 98!", L"HelloMsg", MB_OK );

   return 0;
}
