#include <stdint.h> 
#include <pic32mx.h> 
#include "mipslab.h" 

int getsw( void ){
    int getValue = PORTD;
    return (getValue >> 8) & 0xF;
}

int getbtns( void ){
    int getValue = PORTD;
    return (getValue >> 5) & 0x7;
}

int getotherbtn(void){
    int getValue= PORTF;
    return (getValue>>1)&0x1;
}


