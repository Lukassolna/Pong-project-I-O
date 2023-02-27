/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2023-02-15 by R Finnerman

   For copyright and licensing, see file COPYING */

#include <stdint.h>                        /* Declarations of uint_32 and the like */
#include <pic32mx.h>                       /* Declarations of system-specific addresses etc */
#include "mipslab.h"                       /* Declatations for these labs */


/* Lab-specific initialization goes here */
void labinit(void)
{
  TRISD = TRISD | 0x0fe0; // mask out the bits 5-11 and set as '1' = input (keep other values)
  TRISF = TRISF | 0x02; // mask input and set as '1'

  T2CONSET = 0x70;             // Sets prescaling to 1:256 by setting bit 4-6 to '111' which = prescale 256
  PR2 = (80000000 / 256) / 10; // Decide for how long the timer should be going
  TMR2 = 0x0;                  // Initial value for timer2
  T2CONSET = 0x8000;           // Start timer by changing bit 15

  return;
}