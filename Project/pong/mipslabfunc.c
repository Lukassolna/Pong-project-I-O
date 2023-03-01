/* mipslabfunc.c
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson
   File modified 2023, by Rasmus Finnerman and Lukas Jaokbsén
   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

/* Declare a helper function which is local to this file */

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)
/* quicksleep:
   A simple function to create a small delay.
   Very inefficient use of computing resources,
   but very handy in some special cases. */
void quicksleep(int cyc) {
  int i;
  for(i = cyc; i > 0; i--);
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

// All of the code below is written by Rasmus Finnerman and Lukas Jaokbsén

void convertToDisplay(int row, int column) {
  int printableRow = row / 8;
  int byteRepresentation = 1 << (row % 8);
  display[128 * printableRow + column] &= ~byteRepresentation;
}

void clearDisplayBit(int row, int column) {
  int printableRow = row / 8;
  int byteRepresentation = 1 << (row % 8);
  display[128 * printableRow + column] |= byteRepresentation; //set the bit to '0' with bitwise OR
}

//welcome message that is displayed before the game starts
void welcome () {
  display_string(0, "Welcome to");
	display_string(1, "Pong Game");
	display_string(2, "by Rasmus and");
  display_string(3, "Lukas");

  display_update(); // update the display to show the display_string
  quicksleep(40000000);  //A small delay for the message
}

int paddlePos = 0;
int paddle2pos = 0;

//function to draw the paddles on the display
void drawPaddle(int y) {
  int i;
  for (i = 0; i < 7; i++) { 
    convertToDisplay(y + i, 1);
    convertToDisplay(y + i, 126);
  }
    paddlePos = y + 3;
    paddle2pos = y + 3;
    display_image(0, display);
}

// update paddle position based on button value
void paddles( void )
{
   // detect buttons
  int buttonValue = getbtns();  //get PORTD buttons 2-4
  int otherValue = getotherbtn(); //get PORTF button 1
 
  // Paddle 1
  if ((buttonValue & 4) && (paddlePos<= 27)){ // button 1 pressed
   convertToDisplay(paddlePos + 4, 1);
   clearDisplayBit(paddlePos - 3, 1);
   paddlePos += 1;
  }

  if ((buttonValue & 2) && (paddlePos>=4)) { // button 2 pressed
   convertToDisplay(paddlePos - 4, 1);
   clearDisplayBit(paddlePos + 3, 1);
   paddlePos -= 1;
  }

  // Paddle 2
  if ((buttonValue & 1) && (paddle2pos <= 27)){ // button 3 pressed
   convertToDisplay(paddle2pos + 4, 126);
   clearDisplayBit(paddle2pos - 3, 126);
   paddle2pos += 1;
  }

  if ((otherValue & 1) && (paddle2pos>=4)) { // button 4 pressed
   convertToDisplay(paddle2pos - 4, 126);
   clearDisplayBit(paddle2pos + 3, 126);
   paddle2pos -= 1;
  }
 }

// change the speed of the game by making the delay lower / higher
// initial speed is set to normal speed and switch 4 will make it faster
int speed = 220000;

void changeSpeed () {
  int switchValue = getsw();
  if(switchValue & 8){  //switch 4
    speed = 100000;
  }
  if(switchValue == 0){
    speed = 220000;
  }
}

// Check winner and display a message
int whoWon;
void checkWinner(){
  if (whoWon == 1)  //player 1
  {
  display_string(0,"CONGRATULATIONS ");
  display_string(1,"Player 1");
  display_string(2,"");
  display_string(3,"        ('_')");
  }
 
  if (whoWon == 2){ //player 2
  display_string(0,"CONGRATULATIONS ");
  display_string(1,"Player 2 ");
  display_string(2,"");
  display_string(3,"        ('_')");
  }
}

// function to make the leds flash
// flashes = number of flashes. delay = the delay between the flashes
void flashLeds (int flashes, int delay){
  int i;
  for(i = 0; i < flashes; i++) {
    TRISE = TRISE & 0xffffff00;
    PORTE = 255;  // set value to 255 to turn on all the leds
    quicksleep(delay * 10000);
    PORTE = 0;  // turn off all the leds
    quicksleep(delay * 10000);
  }
}

int x = 63; //set ball 'x' initial coordinate
int y = 25; //set ball 'y' initial coordinate

int xSpeed = 1; //set initial x speed
int ySpeed = 1; //set initial y speed

void updateBall(){
  y += ySpeed;  
  x += xSpeed;

   // Check if the ball has hit roof or floor and change direction
    if (y == 0 || y == 31) {
        ySpeed = -ySpeed;
    }

    // Check if the ball has hit either of the paddles and change direction
    if ((x == 125 ) && (y >= paddle2pos - 3 && y <= paddle2pos + 3)) {
        xSpeed = -xSpeed;
        flashLeds(1, 2);
    }
    if ((x == 2 ) && (y >= paddlePos - 3 && y <= paddlePos + 3)) {
        xSpeed = -xSpeed;
        flashLeds(1, 2);
    }
    // check if the ball has hit left or right side and declare a winner
    if (x == 126){  // right side
      whoWon = 1; // player 1 = winner
    }

    if (x == 0){ // left side
      whoWon = 2; // player 2 = winner
    }
}
void startGame(){
  flashLeds(5, 100); //flash the leds before the game starts
  drawPaddle(10);	//draw paddles

while (whoWon == 0) {
  changeSpeed();  //checks switch value to change speed of the game
  updateBall(); // Update ball position

  convertToDisplay(y, x);  //moves ball
  paddles();  //move paddle if buttons are pressed
  display_image(0,display); //writes to the display (ball and the paddles)
  clearDisplayBit(y, x); //removes the old ball position
  quicksleep(speed);  //set the speed of the game by changing delay
  }

checkWinner();  // checks winner
flashLeds(10, 10);
display_update();
}
