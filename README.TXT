The source file "cputimes.c" was obtained directly from the ATT Bell Lab
FTP site (netlib.att.com). I have compiled three versions of it:

cputime1        IBM C/C++, Version 2, default settings

cputime2        IBM C/C++, Version 2, /Gi+ /O+ switches

cputime3        Microsoft C for OS/2, Version 6.0A, large model, defaults

The program itself is discussed in the article "A Simple Cost Model for C",
(Unix Review, Feb 1991) by Bently, Kernighan, and Van Wyk.

The only significant change needed to make the code compile and run under
OS/2 was the replacement of the "close()" calls with "fclose()". You may
want to fiddle with the constants a bit, I left them as-is.

Have fun.

John M. Hughes
jmh@noao.edu
jmh%cerberus.uucp@noao.edu