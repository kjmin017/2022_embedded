#include "stm32f10x.h"

const int PD2=0X4; //pd2
const int PD3=0x8; //pd3
const int PD4=0x10;//pd4
const int PD7=0x80;//pd7

void off(){
  
  (*(volatile unsigned *)0x40011410) = ~(PD2|PD3|PD4|PD7);
}

void on2(){
  (*(volatile unsigned *)0x40011410) |= PD2;
}

void on3(){
  (*(volatile unsigned *)0x40011410) |= PD3;
}

void on4(){
  (*(volatile unsigned *)0x40011410) |= PD4;
}

void on7(){ 
  (*(volatile unsigned *)0x40011410) |= PD7;
}



int main(void)
{
  (*(volatile unsigned *)0x40021018) |= 0x30;
  
  (*(volatile unsigned *)0x40011400) &= ~0x10011100;//RESET
  (*(volatile unsigned *)0x40011404) = 0x44444444;
  (*(volatile unsigned *)0x40011008) = 0x00000000;
  
  (*(volatile unsigned *)0x40011400) = 0x10011100;//LED ON
  
  while(1){
    if(~(*(volatile unsigned *)0x40011008) & 0x20){//up
      on2();
    }
    else if(~(*(volatile unsigned *)0x40011008) & 0x4){//down
      on3();
    }
    else if(~(*(volatile unsigned *)0x40011008) & 0x8){//left
      on4();
    }
    else if(~(*(volatile unsigned *)0x40011008) & 0x10){//right
      on7();
    }
    else
      off();
  }
  
  return 0;
}