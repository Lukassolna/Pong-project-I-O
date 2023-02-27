/* mipslabfunc.c
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

/* Declare a helper function which is local to this file */
static void num32asc( char * s, int ); 

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)
uint8_t dubbelArray[32][128];
/* quicksleep:
   A simple function to create a small delay.
   Very inefficient use of computing resources,
   but very handy in some special cases. */
void quicksleep(int cyc) {
	int i;
	for(i = cyc; i > 0; i--);
}

/* tick:
   Add 1 to time in memory, at location pointed to by parameter.
   Time is stored as 4 pairs of 2 NBCD-digits.
   1st pair (most significant byte) counts days.
   2nd pair counts hours.
   3rd pair counts minutes.
   4th pair (least significant byte) counts seconds.
   In most labs, only the 3rd and 4th pairs are used. */
void tick( unsigned int * timep )
{
  /* Get current value, store locally */
  register unsigned int t = * timep;
  t += 1; /* Increment local copy */
  
  /* If result was not a valid BCD-coded time, adjust now */

  if( (t & 0x0000000f) >= 0x0000000a ) t += 0x00000006;
  if( (t & 0x000000f0) >= 0x00000060 ) t += 0x000000a0;
  /* Seconds are now OK */

  if( (t & 0x00000f00) >= 0x00000a00 ) t += 0x00000600;
  if( (t & 0x0000f000) >= 0x00006000 ) t += 0x0000a000;
  /* Minutes are now OK */

  if( (t & 0x000f0000) >= 0x000a0000 ) t += 0x00060000;
  if( (t & 0x00ff0000) >= 0x00240000 ) t += 0x00dc0000;
  /* Hours are now OK */

  if( (t & 0x0f000000) >= 0x0a000000 ) t += 0x06000000;
  if( (t & 0xf0000000) >= 0xa0000000 ) t = 0;
  /* Days are now OK */

  * timep = t; /* Store new value */
}

/* display_debug
   A function to help debugging.

   After calling display_debug,
   the two middle lines of the display show
   an address and its current contents.

   There's one parameter: the address to read and display.

   Note: When you use this function, you should comment out any
   repeated calls to display_image; display_image overwrites
   about half of the digits shown by display_debug.
*/   
void display_debug( volatile int * const addr )
{
  display_string( 1, "Addr" );
  display_string( 2, "Data" );
  num32asc( &textbuffer[1][6], (int) addr );
  num32asc( &textbuffer[2][6], *addr );
  display_update();
}

uint8_t spi_send_recv(uint8_t data) {
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 1));
	return SPI2BUF;
}

void display_init(void) {
        DISPLAY_CHANGE_TO_COMMAND_MODE;
	quicksleep(10);
	DISPLAY_ACTIVATE_VDD;
	quicksleep(1000000);
	
	spi_send_recv(0xAE);
	DISPLAY_ACTIVATE_RESET;
	quicksleep(10);
	DISPLAY_DO_NOT_RESET;
	quicksleep(10);
	
	spi_send_recv(0x8D);
	spi_send_recv(0x14);
	
	spi_send_recv(0xD9);
	spi_send_recv(0xF1);
	
	DISPLAY_ACTIVATE_VBAT;
	quicksleep(10000000);
	
	spi_send_recv(0xA1);
	spi_send_recv(0xC8);
	
	spi_send_recv(0xDA);
	spi_send_recv(0x20);
	
	spi_send_recv(0xAF);
}

void display_string(int line, char *s) {
	int i;
	if(line < 0 || line >= 4)
		return;
	if(!s)
		return;
	
	for(i = 0; i < 16; i++)
		if(*s) {
			textbuffer[line][i] = *s;
			s++;
		} else
			textbuffer[line][i] = ' ';
}

void display_image(int x, const uint8_t *data) {
	int i, j;

	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;

		spi_send_recv(0x22);
		spi_send_recv(i);

		spi_send_recv(x & 0xF);
		spi_send_recv(0x10 | ((x >> 4) & 0xF));

		DISPLAY_CHANGE_TO_DATA_MODE;

		// Adding multiplier of 4 so we calculate with the whole width (4 "pages")
		for(j = 0; j < 32*4; j++)
			spi_send_recv(~data[i*32*4 + j]);
	}
}


void display_update(void) {
	int i, j, k;
	int c;
	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;
		spi_send_recv(0x22);
		spi_send_recv(i);
		
		spi_send_recv(0x0);
		spi_send_recv(0x10);
		
		DISPLAY_CHANGE_TO_DATA_MODE;
		
		for(j = 0; j < 16; j++) {
			c = textbuffer[i][j];
			if(c & 0x80)
				continue;
			
			for(k = 0; k < 8; k++)
				spi_send_recv(font[c*8 + k]);
		}
	}
}

/* Helper function, local to this file.
   Converts a number to hexadecimal ASCII digits. */
static void num32asc( char * s, int n ) 
{
  int i;
  for( i = 28; i >= 0; i -= 4 )
    *s++ = "0123456789ABCDEF"[ (n >> i) & 15 ];
}





// PROJECT CODE STARTS HERE


void convertToDisplay(int row, int column) {
    
    

    int printableRow = row / 8;
    int byteRepresentation = 1 << (row % 8);


    display[128 * printableRow + column] &= ~byteRepresentation;

  // Trying this to convert it into a 2darray to check which values are occupied

  dubbelArray[row][column]=1;
}

void clearDisplayBit(int row, int column) {
    int printableRow = row / 8;
    int byteRepresentation = 1 << (row % 8);

    // Set the bit to 0 by bitwise OR
    display[128 * printableRow + column] |= byteRepresentation;

    // Update the 2D array as well
    dubbelArray[row][column] = 0;
}


int paddlePos=0;
int paddle2pos=0;
void drawPaddle(int x) {
    int i;
    for (i = 0; i < 7; i++) {
        

      
        convertToDisplay(x+i, 1);
        convertToDisplay(x+i, 126);
    }
    paddlePos=x+3;
    paddle2pos=x+3;
}

void labwork( void )
{
   // initialize paddle position
  int buttonValue=getbtns();
  int otherValue=getotherbtn();
  int switchValue=getsw();
  
  // update paddle position based on button value
  if (buttonValue & 4) { // button 1 pressed
   if (paddlePos<=27){
   
    
   convertToDisplay(paddlePos+3,1);
   clearDisplayBit(paddlePos-3,1);
   paddlePos+=1;
    
    
  }
  }
  if (buttonValue & 2) { // button 1 pressed
   if (paddlePos>=4){
   convertToDisplay(paddlePos-3,1);
   clearDisplayBit(paddlePos+3,1);
   paddlePos-=1;
   }
    
    
  }

  /// HERE BEGINS PADDLE 2

  if (buttonValue & 1) { // button 1 pressed
   if (paddle2pos<=27){
   
    
   convertToDisplay(paddle2pos+3,126);
   clearDisplayBit(paddle2pos-3,126);
   paddle2pos+=1;
    
    
  }
  }
  if (otherValue & 1) { // button 1 pressed
   if (paddle2pos>=4){
   convertToDisplay(paddle2pos-3,126);
   clearDisplayBit(paddle2pos+3,126);
   paddle2pos-=1;
   }
    
    
  }
  
    

 }




int whoWon;
void checkWinner(){
  if (whoWon==1)
  {
  display_string(0,"CONGRATULATIONS ");
  display_string(1,"Player 1 ");
  }
  
  if (whoWon==0){
  display_string(0,"CONGRATULATIONS ");
  display_string(1,"Player 2 ");
  }
  
}

void flashLeds (int flashes, int delay){
  int i;
  for(i = 0; i < flashes; i++) {
    TRISE = TRISE & 0xffffff00;
    PORTE = 255;
    quicksleep(delay * 10000);
    PORTE = 0;
    quicksleep(delay * 10000);
  }

}
void updateBall(){
  // 
int y = 10;
int x = 10;

int xSpeed = 1;
int ySpeed = 1;

while (1) {
  
    // Update ball position
    x += xSpeed;
    y += ySpeed;
   // Check if ball has hit roof or floor
    if (x == 0 || x == 31) {
        xSpeed = -xSpeed;
    }

    // Check if ball has hit left or right side of screen
    if (y== 125 ) {
        if (x >= paddle2pos - 3 && x <= paddle2pos + 3){
        ySpeed=-ySpeed;
        flashLeds(1, 2);
        
    }
    }
    if (y==2 ) {
        if (x >= paddlePos - 3 && x <= paddlePos + 3){
        ySpeed=-ySpeed;
        flashLeds(1, 2);
        
        }
    }
    if (y==0){
      whoWon=0;
     break;
    }

     if (y==126){
      whoWon=1;
     break;
    }


  convertToDisplay(x,y);
  labwork();
  display_image(0,display);
  clearDisplayBit(x,y);
  quicksleep(150000);
  //delay(60);
}
checkWinner();
flashLeds(10, 10);
display_update();

}











