// 0.Documentation Section 
// 490.c
// Raul Solorio. Geoffrey Grepo. 10/07/19

#include "SysTick.h"
#include "tm4c123gh6pm.h"

unsigned long volatile x;

void PortC_Init(void);

// PC4 = Step
// PC5 = Dir
void PortC_Init(void){unsigned long volatile delay;
  SYSCTL_RCGC2_R |= 0x00000004;     // 1) C clock
	delay = SYSCTL_RCGC2_R;						// delay
  GPIO_PORTC_CR_R |= 0x30;          // allow changes to PC5-PC4         
	GPIO_PORTC_AMSEL_R &= ~0x30;      // 3) disable analog function for PC5-PC4
	GPIO_PORTC_PCTL_R |= 0x00000000;  // Configure PC5-PC4 to GPIO
  GPIO_PORTC_DIR_R |= 0x30;         // 5) PC5-PC4 output
  GPIO_PORTC_AFSEL_R |= 0x00;       // Select regular I/O on PC5-PC4
  GPIO_PORTC_DEN_R |= 0x30;         // 7) enable digital pins PC5-PC4  
}

int main(void){ 
	PortC_Init(); 													// Controls Step and Dir
	SysTick_Init();													// using 16Mhz clock for delay
  while(1){
		for(x=0;x<200*2;x++){									// motor steps per revolution = 200
			GPIO_PORTC_DATA_R = 0x10;						// motor goes one way. Dir = 0...Step = 1
			SysTick_Wait(16000);								// wait 1000 microseconds (1000*10^-6)  --> (1000*10^6) / (1/16Mhz) = 16000
			GPIO_PORTC_DATA_R = 0x00;						// motor goes same way. Dir = 0... Step = 0 --> needs to see a transition from 1 to 0 to step
			SysTick_Wait(16000);								// wait 1000 microseconds (1000*10^-6)  --> (1000*10^6) / (1/16Mhz) = 16000
		}
	}
}
