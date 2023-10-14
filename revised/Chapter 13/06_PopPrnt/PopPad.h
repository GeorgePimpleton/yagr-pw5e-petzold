#pragma once

/*---------------------------------------
   POPPAD.H -- Popup Editor
   (c) Charles Petzold, 1998
  ---------------------------------------*/

#include <windows.h>
#include <commdlg.h>

// Functions in POPFILE.C
void PopFileInitialize(HWND);
BOOL PopFileOpenDlg(HWND, PWSTR, PWSTR);
BOOL PopFileSaveDlg(HWND, PWSTR, PWSTR);
BOOL PopFileRead(HWND, PWSTR);
BOOL PopFileWrite(HWND, PWSTR);

// Functions in POPFIND.C
HWND PopFindFindDlg(HWND);
HWND PopFindReplaceDlg(HWND);
BOOL PopFindFindText(HWND, int*, LPFINDREPLACEW);
BOOL PopFindReplaceText(HWND, int*, LPFINDREPLACEW);
BOOL PopFindNextText(HWND, int*);
BOOL PopFindValidFind(void);

// Functions in POPFONT.C
void PopFontInitialize(HWND);
BOOL PopFontChooseFont(HWND);
void PopFontSetFont(HWND);
void PopFontDeinitialize(void);

// Functions in POPPRNT.C
BOOL PopPrntPrintFile(HINSTANCE, HWND, HWND, PWSTR);
