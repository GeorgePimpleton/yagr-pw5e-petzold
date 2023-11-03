/*----------------------------------------
   HEXCALC.C -- Hexadecimal Calculator
                (c) Charles Petzold, 1998
  ----------------------------------------*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(_In_     HINSTANCE inst,
                    _In_opt_ HINSTANCE prevInst,
                    _In_     PWSTR     cmdLine,
                    _In_     int       showCmd)
{
   UNREFERENCED_PARAMETER(prevInst);
   UNREFERENCED_PARAMETER(cmdLine);

   PCWSTR   appName = L"HexCalc";
   HWND     wnd;
   MSG      msg;
   WNDCLASS wc      = { 0 };

   wc.style         = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = DLGWINDOWEXTRA;    // Note!
   wc.hInstance     = inst;
   wc.hIcon         = (HICON)   LoadImageW(inst, appName, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wc.hbrBackground = (HBRUSH)  (COLOR_BTNFACE + 1);
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = appName;

   if ( !RegisterClassW(&wc) )
   {
      MessageBoxW(NULL, L"This program requires Windows NT!", appName, MB_ICONERROR);
      return 0;
   }

   wnd = CreateDialogParamW(inst, appName, 0, NULL, 0L);

   ShowWindow(wnd, showCmd);

   while ( GetMessageW(&msg, NULL, 0, 0) )
   {
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
   }
   return (int) msg.wParam;
}

void ShowNumber(HWND wnd, UINT number)
{
   WCHAR buffer[ 20 ];

   wsprintfW(buffer, L"%X", number);
   SetDlgItemTextW(wnd, VK_ESCAPE, buffer);
}

DWORD CalcIt(UINT firstNum, int operation, UINT num)
{
   switch ( operation )
   {
   case '=': return num;
   case '+': return firstNum + num;
   case '-': return firstNum - num;
   case '*': return firstNum * num;
   case '&': return firstNum & num;
   case '|': return firstNum | num;
   case '^': return firstNum ^ num;
   case '<': return firstNum << num;
   case '>': return firstNum >> num;
   case '/': return num ? firstNum / num : MAXDWORD;
   case '%': return num ? firstNum % num : MAXDWORD;
   default: return 0;
   }
}

LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static BOOL newNumber = TRUE;
   static int  operation = '=';
   static UINT number;
   static UINT firstNum;
   HWND        button;

   switch ( msg )
   {
   case WM_KEYDOWN:                   // left arrow --> backspace
      if ( wParam != VK_LEFT )
      {
         break;
      }
      wParam = VK_BACK;
      // fall through

   case WM_CHAR:
      if ( (wParam = (WPARAM) CharUpperW((TCHAR*) wParam)) == VK_RETURN )
      {
         wParam = '=';
      }

      if ( button = GetDlgItem(wnd, wParam) )
      {
         SendMessageW(button, BM_SETSTATE, 1, 0);
         Sleep(100);
         SendMessageW(button, BM_SETSTATE, 0, 0);
      }
      else
      {
         MessageBeep(0);
         break;
      }
      // fall through

   case WM_COMMAND:
      SetFocus(wnd);

      if ( LOWORD(wParam) == VK_BACK )         // backspace
      {
         ShowNumber(wnd, number /= 16);
      }
      else if ( LOWORD(wParam) == VK_ESCAPE )  // escape
      {
         ShowNumber(wnd, number = 0);
      }
      else if ( isxdigit(LOWORD(wParam)) )    // hex digit
      {
         if ( newNumber )
         {
            firstNum = number;
            number = 0;
         }
         newNumber = FALSE;

         if ( number <= MAXDWORD >> 4 )
         {
            ShowNumber(wnd, number = 16 * number + wParam -
                       (isdigit(wParam) ? '0' : 'A' - 10));
         }
         else
         {
            MessageBeep(0);
         }
      }
      else                                    // operation
      {
         if ( !newNumber )
         {
            ShowNumber(wnd, number =
                       CalcIt(firstNum, operation, number));
         }
         newNumber = TRUE;
         operation = LOWORD(wParam);
      }
      return 0;

   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   }
   return DefWindowProcW(wnd, msg, wParam, lParam);
}