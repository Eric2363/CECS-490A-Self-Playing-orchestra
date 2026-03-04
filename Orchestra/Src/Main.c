/*
System Info
Clock: 50MHz

*/

#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "DelayTimer.h"
#include "SPI.h"
#include "Latch.h"




void SystemInit();

int main(){

	SystemInit();

	uint8_t bit = 0x01;

	
	while(1){

		UnLatch();
		SPI_Write(bit);
		Latch_Pulse();
	
		bit = bit <<1;
		
		if(bit == 0){
			bit = 0x01;
		}
		
	DelayMs(500);

	}
	
	

	return 0;
}


void SystemInit(){
	PLL_Init();
	SPI_Init();
	Latch_Init();
	DelayTimer_Init();
	
}