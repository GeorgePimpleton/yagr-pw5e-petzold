# yagr-pw5e-petzold
'Yet Another GitHub Repository' of revised C source code for Charles Petzold's Programming Windows 5th Edition ISBN-10 157231995X, adapting to the modern WinAPI

## Changes to sources, 07_RandRect
One big change to the Random Rectangle example is to use the WinAPI Sleep() function to slow down the rectangle drawing time.  When this example was originally written the CPUs were slower, with modern CPUs the code unmodified draws the rectangles IMO way fast to watch.
