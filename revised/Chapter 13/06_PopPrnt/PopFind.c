/*--------------------------------------------------------
   POPFIND.C -- Popup Editor Search and Replace Functions
  --------------------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <commdlg.h>
#include <malloc.h>

#define MAX_STRING_LEN   256

static WCHAR findText[ MAX_STRING_LEN ];
static WCHAR replText[ MAX_STRING_LEN ];

HWND PopFindFindDlg(HWND wnd)
{
   static FINDREPLACEW fr;       // must be static for modeless dialog!!!

   fr.lStructSize      = sizeof(FINDREPLACEW);
   fr.hwndOwner        = wnd;
   fr.hInstance        = NULL;
   fr.Flags            = FR_HIDEUPDOWN | FR_HIDEMATCHCASE | FR_HIDEWHOLEWORD;
   fr.lpstrFindWhat    = findText;
   fr.lpstrReplaceWith = NULL;
   fr.wFindWhatLen     = MAX_STRING_LEN;
   fr.wReplaceWithLen  = 0;
   fr.lCustData        = 0;
   fr.lpfnHook         = NULL;
   fr.lpTemplateName   = NULL;

   return FindTextW(&fr);
}

HWND PopFindReplaceDlg(HWND wnd)
{
   static FINDREPLACEW fr;       // must be static for modeless dialog!!!

   fr.lStructSize      = sizeof(FINDREPLACEW);
   fr.hwndOwner        = wnd;
   fr.hInstance        = NULL;
   fr.Flags            = FR_HIDEUPDOWN | FR_HIDEMATCHCASE | FR_HIDEWHOLEWORD;
   fr.lpstrFindWhat    = findText;
   fr.lpstrReplaceWith = replText;
   fr.wFindWhatLen     = MAX_STRING_LEN;
   fr.wReplaceWithLen  = MAX_STRING_LEN;
   fr.lCustData        = 0;
   fr.lpfnHook         = NULL;
   fr.lpTemplateName   = NULL;

   return ReplaceTextW(&fr);
}

BOOL PopFindFindText(HWND wndEdit, int* searchOffset, LPFINDREPLACEW fr)
{
   int   length;
   int   pos;
   PWSTR strDoc;
   PWSTR strPos;

   // Read in the edit document

   length = GetWindowTextLengthW(wndEdit);

   if ( NULL == (strDoc = (PWSTR) malloc((length + 1) * sizeof(WCHAR))) )
   {
      return FALSE;
   }

   GetWindowTextW(wndEdit, strDoc, length + 1);

   // Search the document for the find string

   strPos = wcsstr(strDoc + *searchOffset, fr->lpstrFindWhat);

   // Return an error code if the string cannot be found

   if ( strPos == NULL )
   {
      return FALSE;
   }

   // Find the position in the document and the new start offset

   pos           = strPos - strDoc;
   *searchOffset = pos + lstrlenW(fr-> lpstrFindWhat);

   // Select the found text

   SendMessageW(wndEdit, EM_SETSEL, pos, *searchOffset);
   SendMessageW(wndEdit, EM_SCROLLCARET, 0, 0);

   free(strDoc);

   return TRUE;
}

BOOL PopFindNextText(HWND wndEdit, int* searchOffset)
{
   FINDREPLACEW fr;

   fr.lpstrFindWhat = findText;

   return PopFindFindText(wndEdit, searchOffset, &fr);
}

BOOL PopFindReplaceText(HWND wndEdit, int* searchOffset, LPFINDREPLACEW fr)
{
   // Find the text

   if ( !PopFindFindText(wndEdit, searchOffset, fr) )
   {
      return FALSE;
   }

   // Replace it

   SendMessageW(wndEdit, EM_REPLACESEL, 0, (LPARAM) fr->lpstrReplaceWith);

   return TRUE;
}

BOOL PopFindValidFind(void)
{
   return *findText != '\0';
}
