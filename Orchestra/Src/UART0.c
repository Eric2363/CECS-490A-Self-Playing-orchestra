#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "UART0.h"

#define UART0_NVIC_EN   0x20
#define RB_SIZE         64

typedef struct{
  volatile uint16_t head;
  volatile uint16_t tail;
  volatile uint8_t buf[RB_SIZE];
} RingBuffer;

static RingBuffer UART0_RxRB;

// ---------- Ring buffer helpers ----------
static void rb0_Init(RingBuffer *rb){
  rb->head = 0;
  rb->tail = 0;
}

static bool rb0_Empty(const RingBuffer *rb){
  return (rb->head == rb->tail);
}

static bool rb0_Put(RingBuffer *rb, uint8_t data){
  uint16_t next = (rb->head + 1) % RB_SIZE;
  if(next == rb->tail){
    return false;   // buffer full
  }
  rb->buf[rb->head] = data;
  rb->head = next;
  return true;
}

static bool rb0_Get(RingBuffer *rb, uint8_t *data){
  if(rb0_Empty(rb)){
    return false;   // buffer empty
  }
  *data = rb->buf[rb->tail];
  rb->tail = (rb->tail + 1) % RB_SIZE;
  return true;
}

// ---------- UART0 Init ----------
// UART0 on PA0/PA1
// Baud rate: 9600 for 50 MHz clock
// RX interrupt enabled
void UART0_Init(void){
  SYSCTL_RCGCUART_R |= 0x01;   // activate UART0
  SYSCTL_RCGCGPIO_R |= 0x01;   // activate Port A
  while((SYSCTL_PRGPIO_R & 0x01) == 0){}

  rb0_Init(&UART0_RxRB);

  UART0_CTL_R &= ~UART_CTL_UARTEN;   // disable UART during setup

  // 50 MHz clock, 9600 baud
  UART0_IBRD_R = 325;
  UART0_FBRD_R = 33;

  // 8-bit, no parity, one stop bit, FIFOs disabled
  UART0_LCRH_R = UART_LCRH_WLEN_8;

  UART0_ICR_R = 0x7FF;               // clear all interrupt flags
  UART0_IM_R  = 0x00;                // mask all interrupts first

  // Configure PA0, PA1 for UART
  GPIO_PORTA_AFSEL_R |= 0x03;
  GPIO_PORTA_DEN_R   |= 0x03;
  GPIO_PORTA_PCTL_R   = (GPIO_PORTA_PCTL_R & 0xFFFFFF00) | 0x00000011;
  GPIO_PORTA_AMSEL_R &= ~0x03;

  UART0_IM_R |= UART_IM_RXIM;        // arm RX interrupt

  NVIC_PRI1_R = (NVIC_PRI1_R & ~0x0000E000); // priority 0
  NVIC_EN0_R |= UART0_NVIC_EN;               // enable IRQ 5

  UART0_CTL_R |= UART_CTL_RXE | UART_CTL_TXE | UART_CTL_UARTEN;
}

// ---------- RX status ----------
bool UART0_Available(void){
  return !rb0_Empty(&UART0_RxRB);
}

// ---------- Input ----------
uint8_t UART0_InChar(void){
  uint8_t data;
  while(!rb0_Get(&UART0_RxRB, &data)){}
  return data;
}

// ---------- Output ----------
void UART0_OutChar(uint8_t data){
  while((UART0_FR_R & UART_FR_TXFF) != 0){}
  UART0_DR_R = data;
}

void UART0_OutString(const char *pt){
  while(*pt){
    UART0_OutChar((uint8_t)*pt);
    pt++;
  }
}

// ---------- String input ----------
void UART0_InString(char *bufPt, uint16_t max){
  int length = 0;
  char character;

  character = (char)UART0_InChar();
  while(character != CR){
    if(character == BS){
      if(length){
        bufPt--;
        length--;
        UART0_OutChar(BS);
      }
    } else if(length < (max - 1)){
      *bufPt = character;
      bufPt++;
      length++;
      UART0_OutChar((uint8_t)character);
    }
    character = (char)UART0_InChar();
  }
  *bufPt = 0;
}

// ---------- Unsigned decimal input ----------
uint32_t UART0_InUDec(void){
  uint32_t number = 0, length = 0;
  char character;

  character = (char)UART0_InChar();
  while(character != CR){
    if((character >= '0') && (character <= '9')){
      number = 10*number + (character - '0');
      length++;
      UART0_OutChar((uint8_t)character);
    } else if((character == BS) && length){
      number /= 10;
      length--;
      UART0_OutChar(BS);
    }
    character = (char)UART0_InChar();
  }
  return number;
}

// ---------- Unsigned decimal output ----------
void UART0_OutUDec(uint32_t n){
  if(n >= 10){
    UART0_OutUDec(n/10);
    n = n%10;
  }
  UART0_OutChar((uint8_t)(n + '0'));
}

// ---------- CRLF ----------
void UART0_OutCRLF(void){
  UART0_OutChar(CR);
  UART0_OutChar(LF);
}

// ---------- UART0 ISR ----------
// Generic ISR: read all received bytes and place them in RX ring buffer
void UART0_Handler(void){
  uint8_t data;

  if(UART0_RIS_R & UART_RIS_RXRIS){
    while((UART0_FR_R & UART_FR_RXFE) == 0){
      data = (uint8_t)(UART0_DR_R & 0xFF);
      rb0_Put(&UART0_RxRB, data);   // drops byte if buffer is full
    }
    UART0_ICR_R = UART_ICR_RXIC;
  }

  if(UART0_RIS_R & UART_RIS_TXRIS){
    UART0_ICR_R = UART_ICR_TXIC;
  }
}