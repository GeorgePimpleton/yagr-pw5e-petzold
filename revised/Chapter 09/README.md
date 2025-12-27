# yagr-pw5e-petzold
'Yet Another GitHub Repository' of revised C source code for Charles Petzold's Programming Windows 5th Edition ISBN-10 157231995X, adapting to the modern WinAPI

### Note
The original source code for Color1, Environ and Head will compile only for x86.  ¯\\\_(ツ)_/¯

Environ as originally written will still silently fail even when 'successfully' compiled even before the main window opens.  Memory gets corrupted.  The modernization is the only way to go.
