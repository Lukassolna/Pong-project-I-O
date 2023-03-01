#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"
// Code written by Rasmus Finnerman and Lukas Jakobsén

//  function to return status of switches
//  switches 1-4 is connected to bits 11-8 of Port D
int getsw(void)
{
    return (PORTD >> 8) & 0xF; // Shift bits and mask out 4 least significant bits (data from swtiches)
}

// function to return status of buttons
// buttons 2-4 is connected to bits 5, 6, 7 of Pord D
int getbtns(void)
{
    return (PORTD >> 5) & 0x7; // Shift bits and masking out 3 least significant bits (data from buttons)
}

// return status of button 1
int getotherbtn(void)
{
    return (PORTF >> 1 ) & 0x1; //Shift bit and masking out least significant bit (data from button)
}


