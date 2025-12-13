# yagr-pw5e-petzold
Yet Another github repository of revised C source code for Charles Petzold's Programming Windows 5th Edition ISBN-10 157231995X, adapting to the modern WinAPI

## What is this?
Many of the examples still compile, with a bit of modernization, but some don't.  Chapter 16 is a biggie.  Expecting a PC with hardware that can use a 256-bit display pallette is so 1980's.

## Changes to sources
I have explicitly modified the source code from Petzold's original to compile as Unicode, annotated wWinMain with SAL annotations to stop Visual Studio from whinging, and stopped using Hungarian Notation with most (not all) variable names.

I've also modified the directory structure to better reflect the progression of code examples when a chapter has multiple examples.

### Note
This is older Windows Desktop WinAPI code, for modern Windows systems (10 & 11) it can be reported as being incompatible.  The highest compatible system is Win 8.