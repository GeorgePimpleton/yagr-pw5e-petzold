/*------------------------------------------
   POPFILE.C -- Popup Editor File Functions
  ------------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <commdlg.h>
#include <malloc.h>

static OPENFILENAMEW ofn;

void PopFileInitialize(HWND hwnd)
{
   static WCHAR filter[ ] = L"Text Files (*.TXT)\0*.txt\0"  \
                            L"ASCII Files (*.ASC)\0*.asc\0" \
                            L"All Files (*.*)\0*.*\0\0";

   ofn.lStructSize       = sizeof(OPENFILENAMEW);
   ofn.hwndOwner         = hwnd;
   ofn.hInstance         = NULL;
   ofn.lpstrFilter       = filter;
   ofn.lpstrCustomFilter = NULL;
   ofn.nMaxCustFilter    = 0;
   ofn.nFilterIndex      = 0;
   ofn.lpstrFile         = NULL;          // Set in Open and Close functions
   ofn.nMaxFile          = MAX_PATH;
   ofn.lpstrFileTitle    = NULL;          // Set in Open and Close functions
   ofn.nMaxFileTitle     = MAX_PATH;
   ofn.lpstrInitialDir   = NULL;
   ofn.lpstrTitle        = NULL;
   ofn.Flags             = 0;             // Set in Open and Close functions
   ofn.nFileOffset       = 0;
   ofn.nFileExtension    = 0;
   ofn.lpstrDefExt       = L"txt";
   ofn.lCustData         = 0L;
   ofn.lpfnHook          = NULL;
   ofn.lpTemplateName    = NULL;
}

BOOL PopFileOpenDlg(HWND wnd, PWSTR fileName, PWSTR titleName)
{
   ofn.hwndOwner      = wnd;
   ofn.lpstrFile      = fileName;
   ofn.lpstrFileTitle = titleName;
   ofn.Flags          = OFN_HIDEREADONLY | OFN_CREATEPROMPT;

   return GetOpenFileNameW(&ofn);
}

BOOL PopFileSaveDlg(HWND wnd, PTSTR fileName, PTSTR titleName)
{
   ofn.hwndOwner      = wnd;
   ofn.lpstrFile      = fileName;
   ofn.lpstrFileTitle = titleName;
   ofn.Flags          = OFN_OVERWRITEPROMPT;

   return GetSaveFileNameW(&ofn);
}

BOOL PopFileRead(HWND wndEdit, PWSTR fileName)
{
   BYTE   swap;
   DWORD  bytesRead;
   HANDLE file;
   int    i;
   int    fileLength;
   int    uniTest;
   PBYTE  buffer;
   PBYTE  text;
   PBYTE  conv;

   // Open the file.

   if ( INVALID_HANDLE_VALUE == (file = CreateFileW(fileName, GENERIC_READ, FILE_SHARE_READ,
                                                    NULL, OPEN_EXISTING, 0, NULL)) )
   {
      return FALSE;
   }

   // Get file size in bytes and allocate memory for read.
   // Add an extra two bytes for zero termination.

   fileLength = GetFileSize(file, NULL);
   buffer     = malloc(fileLength + 2);

   // Read file and put terminating zeros at end.

   ReadFile(file, buffer, fileLength, &bytesRead, NULL);
   CloseHandle(file);
   buffer[ fileLength ]     = '\0';
   buffer[ fileLength + 1 ] = '\0';

   // Test to see if the text is Unicode

   uniTest = IS_TEXT_UNICODE_SIGNATURE | IS_TEXT_UNICODE_REVERSE_SIGNATURE;

   if ( IsTextUnicode(buffer, fileLength, &uniTest) )
   {
      text        = buffer + 2;
      fileLength -= 2;

      if ( uniTest & IS_TEXT_UNICODE_REVERSE_SIGNATURE )
      {
         for ( i = 0; i < fileLength / 2; i++ )
         {
            swap                        = ((BYTE*) text)[ 2 * i ];
            ((BYTE*) text)[ 2 * i ]     = ((BYTE*) text)[ 2 * i + 1 ];
            ((BYTE*) text)[ 2 * i + 1 ] = swap;
         }
      }

      // Allocate memory for possibly converted string
      conv = malloc(fileLength + 2);

      // If the edit control is not Unicode, convert Unicode text to
      // non-Unicode (ie, in general, wide character).
#ifndef UNICODE
      WideCharToMultiByte(CP_ACP, 0, (PWSTR) text, -1, conv,
                          fileLength + 2, NULL, NULL);

      // If the edit control is Unicode, just copy the string
#else
      lstrcpyW((PWSTR) conv, (PWSTR) text);
#endif

   }
   else      // the file is not Unicode
   {
      text = buffer;

      // Allocate memory for possibly converted string.

      conv = malloc(2 * fileLength + 2);

      // If the edit control is Unicode, convert ASCII text.
#ifdef UNICODE
      MultiByteToWideChar(CP_ACP, 0, text, -1, (PWSTR) conv,
                          fileLength + 1);

      // If not, just copy buffer
#else
      lstrcpyW((PWSTR) conv, (PWSTR) text);
#endif
   }

   SetWindowTextW(wndEdit, (PWSTR) conv);
   free(buffer);
   free(conv);

   return TRUE;
}

BOOL PopFileWrite(HWND wndEdit, PTSTR fileName)
{
   DWORD  bytesWritten;
   HANDLE file;
   int    length;
   PWSTR  buffer;
   WORD   byteOrderMark = 0xFEFF;

   // Open the file, creating it if necessary

   if ( INVALID_HANDLE_VALUE == (file = CreateFileW(fileName, GENERIC_WRITE, 0,
                                                    NULL, CREATE_ALWAYS, 0, NULL)) )
   {
      return FALSE;
   }

   // Get the number of characters in the edit control and allocate
   // memory for them.
   length = GetWindowTextLengthW(wndEdit);
   buffer = (PWSTR) malloc((length + 1) * sizeof(TCHAR));

   if ( !buffer )
   {
      CloseHandle(file);
      return FALSE;
   }

   // If the edit control will return Unicode text, write the
   // byte order mark to the file.
#ifdef UNICODE
   WriteFile(file, &byteOrderMark, 2, &bytesWritten, NULL);
#endif

   // Get the edit buffer and write that out to the file.
   GetWindowTextW(wndEdit, buffer, length + 1);
   WriteFile(file, buffer, length * sizeof(TCHAR),
             &bytesWritten, NULL);

   if ( (length * sizeof(TCHAR)) != (int) bytesWritten )
   {
      CloseHandle(file);
      free(buffer);
      return FALSE;
   }

   CloseHandle(file);
   free(buffer);

   return TRUE;
}