 /*mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog 

   For copyright and licensing, see file COPYING */

#include <stdint.h>   
#include <pic32mx.h>
#include "mipslab.h"  

int mytime = 0x5957;
int counter11 =0;

char textstring[] = "text, more text, and even more text!";
volatile int* Porte;
volatile int* Trise;

/* Interrupt Service Routine */
void user_isr( void )
{
  return;
}


void labinit( void )

{
 Trise=(volatile int*)0xbf886100;
*Trise &= 0xffffff00;

Porte=(volatile int*)0xbf886110;
*Porte = 0;
TRISD |= 0x0FE0;

 T2CONSET = 0x70; //Sets prescaling to 1:256 genom att sätta bit 4-6 till 111 vilket betyder prescaling 1:256
  PR2=(80000000 / 256) / 10; //Bestämmer hur länge timern ska hålla på
  TMR2 = 0x0; //start värde för timer2
  T2CONSET = 0x8000; //Start timer genom att ändra bit 15


  return;
}


void labwork( void )
{
  int buttonValue=getbtns();
  int switchValue=getsw();
  if(buttonValue&1){
  mytime = mytime & 0xff0f;
  mytime = (switchValue << 4) | mytime;
  }

  if (buttonValue &2){
    mytime = mytime & 0xf0ff;
    mytime = (switchValue << 8) | mytime;
  }

   if (buttonValue & 4) {
    mytime = mytime & 0x0fff;
    mytime = (switchValue << 12) | mytime;

  }



if (IFS(0) & 0x100) { //Checking if ninth bit is equal to one
        // Reset all event flags.
  IFS(0) = 0; 
  counter11++;

 

 if (counter11==10){
    
  time2string( textstring, mytime );
  display_string( 3, textstring );
  display_update();
  tick( &mytime );
  *Porte+=1;
  display_image(96, icon);
  counter11=0;

 }
 
  
  

}
}


// #1  IFS 0 = 0
// #2 Då blir det ingen delay för att if conditionet vi kollar i slutet är alltid uppfyllt och koden vi skrev innan spelar ingen roll
// #3