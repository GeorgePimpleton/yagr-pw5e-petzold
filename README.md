# yagr-pw5e-petzold (Yet Another github repository for Charles Petzold's "Programming Windows", 5th Edition)
A github repository of revised C source code for Charles Petzold's Programming Windows 5th Edition ISBN-10 157231995X, adapting to the modern WinAPI

----

"Programming Windows" 5th Edition was originally published in 1998, when Windows 9X, Windows NT, 16- & 32-bit CPUs and Internet Explorer were in common use.  PW5E was considered by many to be **The Bible** for learning Windows programming.

A lot has changed since then, including the underlying core Windows API technology and programming philosophy.  This repository is an attempt to update the code to modern coding practices, while still retaining the Desktop programming idea.

A newer edition (6th) was published that was targeted for newer WinAPI technologies, devices other than desktop PCs and focusing on Windows 8.  Using XAML and C#, with several examples rewritten in C++.  The 5th Edition was the last to be pure C based.

Many programmers learned Windows programming using these books, and are still used as a decent start for learning.

## Changes to sources
Win9X was a hybrid OS that combined 16-bit functionality with 32-bit.  WinNT was pure 32-bit.  16-bit functions are ANSI, 32-bit are now classified as Unicode.  Modern Windows with WinXP and later is now pure Unicode, running on both 32- and 64-bit CPUs.  The two biggest changes I make are:

1. Explicitly coding WinAPI functions so the Unicode versions are called.  A few samples require ANSI functionality be used for key parts, when dealing with text files.
2. Adapting the code so it compiles (and runs) on x86 & x64 CPUs.  Leveraging a modern multi-core CPU is obviously not addressed since such a CPU didn't exist when the code was originally written, even though Windows can and did utilize multitasking and multithreading.

## Considerations on adapting old source code
Most of the samples after modernizing still work, a credit on Microsoft and and their emphasis on backwards and forwards compatibility.

Several of the samples require a bit more work, some samples AFAIK won't execute no matter what.  Chapter 16 in particular because of the use of 256-bit display palletes, a bygone relic of archaic computing hardware that was common in the Win9X days.

The sources were adapted and compiled using Visual Studio 2022, the Community edition.  It is free for non-commercial use.

## Contents of this repository
There is an archive of the original sources from the CD that was included with the book when purchased, and (of course) the revised/modernized sources.  I've renamed to folders containing the sources to better reflect the orders the code was talked about in the book.

The book's chapters are:
1.  Getting Started
2.  An Introduction to Unicode
3.  Windows and Messages
4.  An Exercise in Text Output
5.  Basic Drawing
6.  The Keyboard
7.  The Mouse
8.  The Timer
9.  Child Window Controls
10. Menus and Other Resources
11. Dialog Boxes
12. The Clipboard
13. Using the Printer
14. Bitmaps and BitBlts
15. The Device-Independent Bitmap
16. The Palette Manager
17. Text and Fonts
18. Metafiles
19. The Multiple-Document Interface
20. Multitasking and Multithreading
21. Dynamic-Link Libraries
22. Sound and Music
23. A Taste of the Internet

## Programming Windows using MFC
There is a companion book to Petzold's PW5E that shows how to code the Win9X Win32 API utilizing MFC and C++, encapsulating the WinAPI in C++ classes.  This repo doesn't use that programming technology.

## Notes
There may be links to websites or internet sources in this repo. Links can  go stale. A search engine is your friend.

### Post-script
The code as as orginally written \****is***\* compileable as written as long as the code isn't compiled using the Unicode/wide character set. With a modern compiler If someone compiled the code using the Multi-byte character set the code needs zero modification.

Using a modern compiler such as 2022 or 2026 (really recommended for Desktop WinAPI code), compiling using the Multi-byte character set both x64 and x86 are doable.  There will be at least two warnings compiling the unmodified code this way, there should be no errors.