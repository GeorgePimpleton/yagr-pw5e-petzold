/*-------------------------------------
   GETPRNDC.C -- GetPrinterDC function
  -------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winspool.h>
#include <malloc.h>

HDC GetPrinterDC(void)
{
   /* The original GetPrinterDC() function checked  for Windows 98
    * or Windows NT. */
   DWORD needed;
   DWORD returned;

   EnumPrintersW(PRINTER_ENUM_LOCAL, NULL, 4, NULL, 0, &needed, &returned);

   PRINTER_INFO_4* pinfo4 = (PRINTER_INFO_4*) malloc(needed);

   EnumPrintersW(PRINTER_ENUM_LOCAL, NULL, 4, (PBYTE)pinfo4, needed, &needed, &returned);

   HDC dc = CreateDCW(NULL, pinfo4->pPrinterName, NULL, NULL);

   free(pinfo4);

   return dc;
}