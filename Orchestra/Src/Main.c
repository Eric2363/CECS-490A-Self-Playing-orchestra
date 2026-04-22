/*
System Info
Clock: 50MHz

*/

#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "DelayTimer.h"
#include "SPI.h"
#include "Latch.h"
#include "MIDI.h"
#include "UART0.h"



void SystemInit();
void ShiftTest();
void NoteOn(uint8_t note);
void NoteOff(uint8_t note);
void UpdateReg(uint8_t Byte0, uint8_t Byte1, uint8_t Byte2, uint8_t Byte3);

//Depending on how many notes needed bit size can increase
//1 Octave - 12 Notes - 16 bits - 4 bits unused
//2 Octave - 24 Notes - 32 bits - 8 bits unused
//3 Octave - 36 Notes - 64 bits - 16 bits unused

uint64_t ORGANSTATE = 0x44332211;

uint8_t bit = 0x01;
int main(){

	SystemInit();

	uint8_t Byte0;
	uint8_t Byte1;
	uint8_t Byte2;
	uint8_t Byte3;
	

	
	while(1){
		
		if(UART0_Available()){
			
			uint8_t data = UART0_InChar();
			
			if(data == 0x09){
				
				data = UART0_InChar();
				
				NoteOn(data);
			}
			else if(data == 0x08){
				data = UART0_InChar();
				NoteOff(data);
				
			}
			
		}
		
		
		//Shift ORGANSTATE into 8 BYTE format and extract information
		Byte0 = (ORGANSTATE & 0xFF);
		Byte1 = (uint8_t)((ORGANSTATE >> 8) & 0xFF);
		Byte2 = (uint8_t) ((ORGANSTATE >> 16)& 0xFF);
		Byte3 = (uint8_t)((ORGANSTATE >> 24)& 0xFF);
		
		//update the reg
		UpdateReg(Byte0,Byte1,Byte2,Byte3);
		


	}
	
	

	return 0;
}
//Update Reg
void UpdateReg(uint8_t Byte0, uint8_t Byte1, uint8_t Byte2, uint8_t Byte3){

	//when using more regs, will need to change to only update once
	//unlatch then write data to reg, then latch to update reg
	UnLatch();
	SPI_Write(Byte0);
	Latch_Pulse();
	DelayMs(2000);
	
	UnLatch();
	SPI_Write(Byte1);
	Latch_Pulse();
	DelayMs(2000);

	UnLatch();
	SPI_Write(Byte2);
	Latch_Pulse();
	DelayMs(2000);	
	
	UnLatch();
	SPI_Write(Byte3);
	Latch_Pulse();
	DelayMs(2000);	
}

void NoteOn(uint8_t note){
    ORGANSTATE |= ((uint64_t)1 << LUT(note));
}

void NoteOff(uint8_t note){
    ORGANSTATE &= ~((uint64_t)1 << LUT(note));
}
void SystemInit(){
	PLL_Init();
	SPI_Init();
	Latch_Init();
	DelayTimer_Init();
	
}