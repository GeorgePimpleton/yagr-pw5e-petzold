/*--------------------------------------------------------------
   HelloMsg.c -- Displays "Hello, Windows 98!" in a message box
             (c) Charles Petzold, 1998
  --------------------------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

int WINAPI wWinMain(_In_     HINSTANCE hInstance,
                    _In_opt_ HINSTANCE hPrevInstance,
                    _In_     PWSTR     pCmdLine,
                    _In_     int       nShowCmd)
{
   UNREFERENCED_PARAMETER(hInstance);
   UNREFERENCED_PARAMETER(hPrevInstance);
   UNREFERENCED_PARAMETER(pCmdLine);
   UNREFERENCED_PARAMETER(nShowCmd);

   MessageBoxW(NULL, L"Hello, Windows 98!", L"HelloMsg", MB_OK);

   return 0;
}