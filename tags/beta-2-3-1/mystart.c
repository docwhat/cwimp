/* $Id$ */
/* This file and it's information is in the PublicDomain.
   This is information on how to *bend* the 32k limit on Palms.
   Originally from: http://www.geocities.com/SiliconValley/Lab/9981/gcctech.htm
   Rewritten for use in CWimp by Christian Höltje
*/

/***
 *** MORE THAN 32K of CODE!
 ***
 *

 It is actually very possible and easy to have more than 32k of
 code.  You see, the branch command (aka jump) can only jump
 to code within 32k of the caller.  Well, that gives you 32k
 before and 32k after, for a total of 64k.

 PalmOS runs the 0th bit of your program to get it started.
 This code stub will call PilotMain() and your program will
 then run.  With GCC, this stub is called start().

 It just happens that on the palm pilots with GCC, it puts
 crt0.o (with start() ) at the beginning, and some hooks at
 the end of your code.  Well, that's a problem because you
 are then limited the 32k in the middle since start() 
 *must* be able to call (branch to) the hooks.

 So, the trick is to tell GCC *not* put crt0.o in your binary
 (we'll do it manually) and put our own start() stub in the
 beginning.  The goal is to put start() in the *middle* of
 your program!

     mystart()
     32k of your code
     start()
     32k of your code
     hooks

 First off, you'll need to alter your pilot.ld file.  This
 should be in your gcc-lib for your pilot version of GCC.
 On Debian with prc-tools version 0.5.0r-3.1 it is in:
   /usr/lib/gcc-lib/m68k-palmos-coff/2.7.2.2-kgpd-071097/pilot.ld

 You need to change LENGTH for coderes from 32767 to 65535

 Next, make the mystart() code stub (which is this file).

 Then you need to do the link step of your build using the
 -nostartfiles switch and then include crt0.o with your
 list of objects.  Preferably someplace near the middle,
 size-wise.

 $(CC) $(LDFLAGS) -nostartfiles mystart.o <32k of objects> $(CRT0) <32k of objects> $(LIBS) -o<yourprog>

 You'll have to find and set $(CRT0) yourself.  With my
 Debian setup it's in /usr/m68k-palmos-coff/lib/crt0.o


 Thats it!

 Ciao!
 <docwhat@gerf.org>

Replacement pilot.ld for prc-tools 0.5.0r-3.1:
----------------->8--------CUT HERE-------8<----------------
MEMORY 
        {
        coderes   : ORIGIN = 0x10000, LENGTH = 65535
        datares   : ORIGIN = 0x0, LENGTH = 32767
        }

SECTIONS
{
        .text :
        {
          *(.text)
	  . = ALIGN(4);
	  bhook_start = .;
	  *(bhook)
	  bhook_end = .;
	  . = ALIGN(4);
	  ehook_start = .;
	  *(ehook)
	  ehook_end = .;
        } > coderes
        .data :
        {
  	  data_start = .;
          *(.data)
        } > datares
        .bss :
        {
	bss_start = .;
          *(.bss)
          *(COMMON)
        } > datares
        end = ALIGN( 4 );
        edata = ALIGN( 4 );
}
----------------->8--------CUT HERE-------8<----------------
*/


extern unsigned long start();
unsigned long myStart()
{
  return start();
}
