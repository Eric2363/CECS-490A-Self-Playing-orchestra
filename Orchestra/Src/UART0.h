#include <stdint.h>
#include <stdbool.h>

// Standard ASCII symbols
#define CR   0x0D
#define LF   0x0A
#define BS   0x08
#define ESC  0x1B
#define SP   0x20
#define DEL  0x7F

// UART0 interrupt enable bit in NVIC EN0
#define NVIC_EN0_UART0 0x20

void UART0_Init(void);

bool UART0_Available(void);

uint8_t UART0_InChar(void);
void UART0_OutChar(uint8_t data);
void UART0_OutString(const char *pt);

void UART0_InString(char *bufPt, uint16_t max);

uint32_t UART0_InUDec(void);
void UART0_OutUDec(uint32_t n);

void UART0_OutCRLF(void);

void UART0_Handler(void);