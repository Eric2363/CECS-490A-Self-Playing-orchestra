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
#include "MOSFET.h"

//Constants
#define XYLO_CH0 0x0
#define ORGAN_CH1 0x1
#define DRUM_CH2 0x2
#define ON  0x90
#define OFF 0x80
#define XYLO_START_BIT 0
#define ORGAN_START_BIT 25
#define DRUM_START_BIT 62

//Function prototypes
void SystemInit(void);
void ShiftTest(void);
void midiParser(void);
void NoteOn( uint8_t note);
void NoteOff(uint8_t note);
void UpdateReg(uint8_t Byte0, uint8_t Byte1, uint8_t Byte2, uint8_t Byte3);

//Depending on how many notes needed bit size can increase
//1 Octave - 12 Notes - 16 bits - 4 bits unused
//2 Octave - 24 Notes - 32 bits - 8 bits unused
//3 Octave - 36 Notes - 64 bits - 16 bits unused

//keeps track of which notes are currently on/ off
uint64_t SYSTEM_STATE = 0x44332211;

uint8_t bit = 0x01;

int main(){

	SystemInit();

	uint8_t Byte0;
	uint8_t Byte1;
	uint8_t Byte2;
	uint8_t Byte3;
	

	
	while(1){
//		midiParser();
//		
//		
//		//Shift SYSTEM_STATE into 8 BYTE format and extract information
//		Byte0 = (SYSTEM_STATE & 0xFF);
//		Byte1 = (uint8_t)((SYSTEM_STATE >> 8) & 0xFF);
//		Byte2 = (uint8_t) ((SYSTEM_STATE >> 16)& 0xFF);
//		Byte3 = (uint8_t)((SYSTEM_STATE >> 24)& 0xFF);
//		
//		//update the reg
//		UpdateReg(Byte0,Byte1,Byte2,Byte3);
		
		Mosfet_On();
		DelayMs(500);
		Mosfet_Off();
		DelayMs(500);


	}
	
	

	return 0;
}
void midiParser(){
	//check if any data is in ring buffer
	if(UART0_Available()){
		
			
		//get status byte
		uint8_t data = UART0_InChar();
		//get on or off command
		uint8_t upperNibble = (data & 0xF0);	
		//get channel data
		uint8_t lowerNibble = (data &0x0F);
		
		//On command
		if(upperNibble == ON){
			
			if(lowerNibble ==XYLO_CH0){
				//update organstate memory for CH0 mapped bits

			}
			else if(lowerNibble == ORGAN_CH1){
				//update organstate memory for CH1 mapped bits

			}
			else if(lowerNibble == DRUM_CH2){
				//update organstate memory for CH2 mapped bits

			}
			
				
			NoteOn(data);
		}
		//off command
		else if(data == OFF){
			data = UART0_InChar();
			NoteOff(data);
		}
			
	}

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
    SYSTEM_STATE |= ((uint64_t)1 << LUT(note));
}

void NoteOff(uint8_t note){
    SYSTEM_STATE &= ~((uint64_t)1 << LUT(note));
}
void SystemInit(){
	PLL_Init();
	SPI_Init();
	Latch_Init();
	DelayTimer_Init();
	Mosfet_Init();
	
}