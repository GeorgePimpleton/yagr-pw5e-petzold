/*-----------------------------------------------
   SYSMETS.H -- System metrics display structure
  -----------------------------------------------*/

#include <windows.h>

struct
{
   int    iIndex;
   PCWSTR szLabel;
   PCWSTR szDesc;
}
sysmetrics[] =
{
   {SM_CXSCREEN,           L"SM_CXSCREEN",            L"Screen width in pixels"},
   {SM_CYSCREEN,           L"SM_CYSCREEN",            L"Screen height in pixels"},
   {SM_CXVSCROLL,          L"SM_CXVSCROLL",           L"Vertical scroll width"},
   {SM_CYHSCROLL,          L"SM_CYHSCROLL",           L"Horizontal scroll height"},
   {SM_CYCAPTION,          L"SM_CYCAPTION",           L"Caption bar height"},
   {SM_CXBORDER,           L"SM_CXBORDER",            L"Window border width"},
   {SM_CYBORDER,           L"SM_CYBORDER",            L"Window border height"},
   {SM_CXFIXEDFRAME,       L"SM_CXFIXEDFRAME",        L"Dialog window frame width"},
   {SM_CYFIXEDFRAME,       L"SM_CYFIXEDFRAME",        L"Dialog window frame height"},
   {SM_CYVTHUMB,           L"SM_CYVTHUMB",            L"Vertical scroll thumb height"},
   {SM_CXHTHUMB,           L"SM_CXHTHUMB",            L"Horizontal scroll thumb width"},
   {SM_CXICON,             L"SM_CXICON",              L"Icon width"},
   {SM_CYICON,             L"SM_CYICON",              L"Icon height"},
   {SM_CXCURSOR,           L"SM_CXCURSOR",            L"Cursor width"},
   {SM_CYCURSOR,           L"SM_CYCURSOR",            L"Cursor height"},
   {SM_CYMENU,             L"SM_CYMENU",              L"Menu bar height"},
   {SM_CXFULLSCREEN,       L"SM_CXFULLSCREEN",        L"Full screen client area width"},
   {SM_CYFULLSCREEN,       L"SM_CYFULLSCREEN",        L"Full screen client area height"},
   {SM_CYKANJIWINDOW,      L"SM_CYKANJIWINDOW",       L"Kanji window height"},
   {SM_MOUSEPRESENT,       L"SM_MOUSEPRESENT",        L"Mouse present flag"},
   {SM_CYVSCROLL,          L"SM_CYVSCROLL",           L"Vertical scroll arrow height"},
   {SM_CXHSCROLL,          L"SM_CXHSCROLL",           L"Horizontal scroll arrow width"},
   {SM_DEBUG,              L"SM_DEBUG",               L"Debug version flag"},
   {SM_SWAPBUTTON,         L"SM_SWAPBUTTON",          L"Mouse buttons swapped flag"},
   {SM_CXMIN,              L"SM_CXMIN",               L"Minimum window width"},
   {SM_CYMIN,              L"SM_CYMIN",               L"Minimum window height"},
   {SM_CXSIZE,             L"SM_CXSIZE",              L"Min/Max/Close button width"},
   {SM_CYSIZE,             L"SM_CYSIZE",              L"Min/Max/Close button height"},
   {SM_CXSIZEFRAME,        L"SM_CXSIZEFRAME",         L"Window sizing frame width"},
   {SM_CYSIZEFRAME,        L"SM_CYSIZEFRAME",         L"Window sizing frame height"},
   {SM_CXMINTRACK,         L"SM_CXMINTRACK",          L"Minimum window tracking width"},
   {SM_CYMINTRACK,         L"SM_CYMINTRACK",          L"Minimum window tracking height"},
   {SM_CXDOUBLECLK,        L"SM_CXDOUBLECLK",         L"Double click x tolerance"},
   {SM_CYDOUBLECLK,        L"SM_CYDOUBLECLK",         L"Double click y tolerance"},
   {SM_CXICONSPACING,      L"SM_CXICONSPACING",       L"Horizontal icon spacing"},
   {SM_CYICONSPACING,      L"SM_CYICONSPACING",       L"Vertical icon spacing"},
   {SM_MENUDROPALIGNMENT,  L"SM_MENUDROPALIGNMENT",   L"Left or right menu drop"},
   {SM_PENWINDOWS,         L"SM_PENWINDOWS",          L"Pen extensions installed"},
   {SM_DBCSENABLED,        L"SM_DBCSENABLED",         L"Double-Byte Char Set enabled"},
   {SM_CMOUSEBUTTONS,      L"SM_CMOUSEBUTTONS",       L"Number of mouse buttons"},
   {SM_SECURE,             L"SM_SECURE",              L"Security present flag"},
   {SM_CXEDGE,             L"SM_CXEDGE",              L"3-D border width"},
   {SM_CYEDGE,             L"SM_CYEDGE",              L"3-D border height"},
   {SM_CXMINSPACING,       L"SM_CXMINSPACING",        L"Minimized window spacing width"},
   {SM_CYMINSPACING,       L"SM_CYMINSPACING",        L"Minimized window spacing height"},
   {SM_CXSMICON,           L"SM_CXSMICON",            L"Small icon width"},
   {SM_CYSMICON,           L"SM_CYSMICON",            L"Small icon height"},
   {SM_CYSMCAPTION,        L"SM_CYSMCAPTION",         L"Small caption height"},
   {SM_CXSMSIZE,           L"SM_CXSMSIZE",            L"Small caption button width"},
   {SM_CYSMSIZE,           L"SM_CYSMSIZE",            L"Small caption button height"},
   {SM_CXMENUSIZE,         L"SM_CXMENUSIZE",          L"Menu bar button width"},
   {SM_CYMENUSIZE,         L"SM_CYMENUSIZE",          L"Menu bar button height"},
   {SM_ARRANGE,            L"SM_ARRANGE",             L"How minimized windows arranged"},
   {SM_CXMINIMIZED,        L"SM_CXMINIMIZED",         L"Minimized window width"},
   {SM_CYMINIMIZED,        L"SM_CYMINIMIZED",         L"Minimized window height"},
   {SM_CXMAXTRACK,         L"SM_CXMAXTRACK",          L"Maximum draggable width"},
   {SM_CYMAXTRACK,         L"SM_CYMAXTRACK",          L"Maximum draggable height"},
   {SM_CXMAXIMIZED,        L"SM_CXMAXIMIZED",         L"Width of maximized window"},
   {SM_CYMAXIMIZED,        L"SM_CYMAXIMIZED",         L"Height of maximized window"},
   {SM_NETWORK,            L"SM_NETWORK",             L"Network present flag"},
   {SM_CLEANBOOT,          L"SM_CLEANBOOT",           L"How system was booted"},
   {SM_CXDRAG,             L"SM_CXDRAG",              L"Avoid drag x tolerance"},
   {SM_CYDRAG,             L"SM_CYDRAG",              L"Avoid drag y tolerance"},
   {SM_SHOWSOUNDS,         L"SM_SHOWSOUNDS",          L"Present sounds visually"},
   {SM_CXMENUCHECK,        L"SM_CXMENUCHECK",         L"Menu check-mark width"},
   {SM_CYMENUCHECK,        L"SM_CYMENUCHECK",         L"Menu check-mark height"},
   {SM_SLOWMACHINE,        L"SM_SLOWMACHINE",         L"Slow processor flag"},
   {SM_MIDEASTENABLED,     L"SM_MIDEASTENABLED",      L"Hebrew and Arabic enabled flag"},
   {SM_MOUSEWHEELPRESENT,  L"SM_MOUSEWHEELPRESENT",   L"Mouse wheel present flag"},
   {SM_XVIRTUALSCREEN,     L"SM_XVIRTUALSCREEN",      L"Virtual screen x origin"},
   {SM_YVIRTUALSCREEN,     L"SM_YVIRTUALSCREEN",      L"Virtual screen y origin"},
   {SM_CXVIRTUALSCREEN,    L"SM_CXVIRTUALSCREEN",     L"Virtual screen width"},
   {SM_CYVIRTUALSCREEN,    L"SM_CYVIRTUALSCREEN",     L"Virtual screen height"},
   {SM_CMONITORS,          L"SM_CMONITORS",           L"Number of monitors"},
   {SM_SAMEDISPLAYFORMAT,  L"SM_SAMEDISPLAYFORMAT",   L"Same color format flag"}
};

#define NUMLINES ((int)(sizeof(sysmetrics) / sizeof(sysmetrics[0])))
