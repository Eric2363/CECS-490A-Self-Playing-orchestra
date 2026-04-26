#include "MOSFET.h"

void Mosfet_Init(){
  //Enable Clock for portB
  SYSCTL_RCGCGPIO_R |= PORTB;
  //wait for it finish set up
  while((SYSCTL_RCGCGPIO_R & PORTB) == 0){}

  //GPIO config
  //Enable B0
  GPIO_PORTB_DEN_R |= LATCH_PIN;
  //B0 as output
  GPIO_PORTB_DIR_R |= LATCH_PIN;
  //Disable Analog mode
  GPIO_PORTB_AMSEL_R &= ~LATCH_PIN;
  //Disable alternate function mode
  GPIO_PORTB_AFSEL_R &= ~LATCH_PIN;

  // start low
  GPIO_PORTB_DATA_R &= ~LATCH_PIN;
}

void Mosfet_On(){
  //set pin high
  GPIO_PORTB_DATA_R |= LATCH_PIN;
}

void Mosfet_Off(){
  //set pin low
  GPIO_PORTB_DATA_R &= ~LATCH_PIN;
}