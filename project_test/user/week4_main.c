#include "stm32f10x.h"

const int PD2=0X4; //pd2
const int PD3=0x8; //pd3
const int PD4=0x10;//pd4
const int PD7=0x80;//pd7
const int PC8=0x100;
const int PC9=0x200;

void delay() {
  int i;
  for (i  = 0; i < 10000000; i++) {}
}

void off(){
  
  (*(volatile unsigned *)0x40011410) = ~(PD2|PD3|PD4|PD7);
  (*(volatile unsigned *)0x40011010) = ~(PC8);
}

void on2(){
  (*(volatile unsigned *)0x40011410) |= PD2; //led1
  delay();
}

void on3(){
  (*(volatile unsigned *)0x40011410) |= PD3;//led2
  delay();
}

void on4(){
  (*(volatile unsigned *)0x40011410) |= PD4; //led3
  delay();
}

void on7(){ 
  //relay(pc8, pc9)
  
  (*(volatile unsigned *)0x40011010) |= 0x100;
  delay();
}



int main(void)
{
  (*(volatile unsigned *)0x40021018) |= 0x38;
  
  (*(volatile unsigned *)0x40011400) &= ~0x10011100;//RESET
  (*(volatile unsigned *)0x40011000) &= 0x44444444;
  (*(volatile unsigned *)0x40011004) &= 0x44444440;
  (*(volatile unsigned *)0x40011404) = 0x44444444;
  
  (*(volatile unsigned *)0x40011008) = 0x00000000;

  (*(volatile unsigned *)0x40011004) |= 0x00000001;
    
  (*(volatile unsigned *)0x40011400) = 0x10011100;//LED ON
  (*(volatile unsigned *)0x40011004) |= 0x100;//relay ON
  
  while(1){
    if(~(*(volatile unsigned *)0x40011008) & 0x20){//up
      on7();
    }
    else if(~(*(volatile unsigned *)0x40011408) & 0x800){//user but1
      on2();
    }
    else if(~(*(volatile unsigned *)0x40011408) & 0x1000){//user but2
      on3();
    }
    else if(~(*(volatile unsigned *)0x40010c08) & 0x100){//selection
      on4();
    }
    else
      off();
  }
  
  return 0;
}
