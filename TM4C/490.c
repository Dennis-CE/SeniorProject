// 0.Documentation Section 
// 490.c
// Raul Solorio. Geoffrey Grepo. 05/02/20
// UART 1 on PC4/PC5 --> PC4 = RX ; PC5 = TX;
// ADC on PE3

#include "SysTick.h"				 // Delays
#include "PLL.h" 						 // overclock to 80MHz
#include "UART.h"						 // UART 1
#include "ADC.h"						 // ADC
#include "tm4c123gh6pm.h"		 // contains all TM4C registers
#include <stdlib.h>					 // for atoi

unsigned long volatile x, i; // used in the for loop
unsigned int sec, rot, rotDeg, degStep, j;  // time to cook
unsigned long ADCpot, inch, sensorFlag = 0; // 12-bit 0 to 4095 sample ; how far the object is in inches, and if an object is in front of sensor

void EnableInterrupts(void);
void WaitForInterrupt(void);
void PortF_Init(void);
void PortB_Init(void);
void PortD_Init(void);
void Delay(unsigned int loops);
void degreeSpin(unsigned char ports, unsigned long volatile motor, unsigned long volatile motor1, unsigned long volatile steps, unsigned long volatile time);

// PA2 = NOT ENABLE --> motor 5 
// PA3 = NOT ENABLE --> motor 6
// PA4 = NOT ENABLE --> motor 7
// PA5 = NOT ENABLE --> motor 8
// PA6 = DIR MOTOR 7
// PA7 = DIR Motor 6
void PortA_Init(void){unsigned long volatile delay;
  SYSCTL_RCGC2_R |= 0x00000001;     // 1) A clock
	delay = SYSCTL_RCGC2_R;						// delay	
  GPIO_PORTA_CR_R |= 0xFF;          // allow changes to PA7-PA0
	GPIO_PORTA_AMSEL_R &= ~0xFF;      // 3) disable analog function for PA7-PA0
	GPIO_PORTA_PCTL_R |= 0x00000000;  // Configure PA7-PA0 to GPIO
	GPIO_PORTA_DIR_R |= 0xFF;         // 5) PA7-PA0 output  
  GPIO_PORTA_AFSEL_R |= 0x00;       // Select regular I/O on PA7-PA0
  GPIO_PORTA_DEN_R |= 0xFF;         // 7) enable digital pins PA7-PA0  
}

// PB0 = Step	--> motor 3
// PB1 = NOT ENABLE --> motor 3
// PB2 = Step --> motor 4
// PB3 = Dir	--> motor 4
// PB4 = NOT ENABLE --> motor 4
// PB5 = Step --> motor 5
// PB6 = Step --> motor 6
void PortB_Init(void){unsigned long volatile delay;
  SYSCTL_RCGC2_R |= 0x00000002;     // 1) B clock
	delay = SYSCTL_RCGC2_R;						// delay
  GPIO_PORTB_CR_R |= 0xFF;          // allow changes to PB7-PB0         
	GPIO_PORTB_AMSEL_R &= ~0xFF;      // 3) disable analog function for PB7-PB0
	GPIO_PORTB_PCTL_R |= 0x00000000;  // Configure PB7-PB0 to GPIO
  GPIO_PORTB_DIR_R |= 0xFF;         // 5) PB7-PB0 output
  GPIO_PORTB_AFSEL_R |= 0x00;       // Select regular I/O on PB7-PB0
  GPIO_PORTB_DEN_R |= 0xFF;         // 7) enable digital pins PB7-PB0  
}

// PD0 = Step	--> motor 7
// PD2 = Step	--> motor 8
// PD3 = Dir	--> Motor 8
// PD6 = Step --> motor 9
// PD7 = NOT ENABLE	--> Motor 9
void PortD_Init(void){unsigned long volatile delay;
  SYSCTL_RCGC2_R |= 0x00000008;     // 1) D clock
	delay = SYSCTL_RCGC2_R;						// delay
	GPIO_PORTD_LOCK_R = 0x4C4F434B;		// unlock PortD PD7
  GPIO_PORTD_CR_R |= 0xFF;          // allow changes to PD7-PD0         
	GPIO_PORTD_AMSEL_R &= ~0xFF;      // 3) disable analog function for PD7-PD0
	GPIO_PORTD_PCTL_R |= 0x00000000;  // Configure PD7-PD0 to GPIO
  GPIO_PORTD_DIR_R |= 0xFF;         // 5) PD7-PD0 output
  GPIO_PORTD_AFSEL_R |= 0x00;       // Select regular I/O on PD7-PD0
  GPIO_PORTD_DEN_R |= 0xFF;         // 7) enable digital pins PD7-PD0  
}

// PortF Initialization fucntion for LED's
void PortF_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) F clock
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTF_CR_R |= 0x0E;          // allow changes to PF3, PF2,PF1       
  GPIO_PORTF_AMSEL_R &= ~0x0E;      // 3) disable analog function
  GPIO_PORTF_PCTL_R &= ~0x0000FFF0; // 4) GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R |= 0x0E;         // 5) PF3,PF2,PF1 for outputs  
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) no alternate function       
  GPIO_PORTF_DEN_R |= 0x0E;         // 7) enable digital pins PF3,PF2,PF1       
}

// Not enable --> 1 to make motor off and 0 to make motor on --> turning off motor prevents the buzzing noise.
void degreeSpin(unsigned char ports, unsigned long volatile motor, unsigned long volatile motor1, unsigned long volatile steps, unsigned long volatile time){
			if(ports == 'B'){											// motors attached to this port 
				for(x=0;x<steps;x++){							  // 360 degrees = 200 steps. Each step goes 1.8 degrees
					GPIO_PORTB_DATA_R = motor;				// STEP, DIRECTION and NOT ENABLE value. Dir = 0 (or 1 for the other direction)..Step = 1. NOT ENABLE = 0 to turn on motor and 1 to turn off.
					SysTick_Wait(time);								// waits a certain amount of time. Ex:1000 microseconds (1000*10^-6)  --> (1000*10^-6) / (1/80Mhz) = 80000 --> 80Mhz is from clock
					GPIO_PORTB_DATA_R = motor1;				// motors goes same way. Dir = 0 (or 1)... Step = 0 --> needs to see a transition from 1 to 0 to step
					SysTick_Wait(time);								// Wait
				}
			}
			else if(ports == 'D'){								// motors attached to this port
				for(x=0;x<steps;x++){							  // comment the same as above
					GPIO_PORTD_DATA_R = motor;				// STEP, DIRECTION and NOT ENABLE value
					SysTick_Wait(time);								// wait
					GPIO_PORTD_DATA_R = motor1;				// Should only change STEP value
					SysTick_Wait(time);								// wait
				}				
			}	
}

void Delay(unsigned int loops){
	for(i=0;i<loops;i++){										  // wait this much time
		SysTick_Wait(16777203);	  			  		  // wait .21 second --> did trial and error
	}
}

// Initialize SysTick interrupts to trigger at 20 Hz, 50 ms
void SysTick_Init(unsigned long period){
  NVIC_ST_CTRL_R = 0;          // disable SysTick during setup
  NVIC_ST_RELOAD_R = period-1; // reload value
  NVIC_ST_CURRENT_R = 0;       // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000; // priority 2
  NVIC_ST_CTRL_R = 0x07;			 // enable SysTick with core clock and interrupts
}

// executes every 50 ms, collects a sample, converts and stores in mailbox (based on sensor datasheet)
void SysTick_Handler(void){ 								// ADC sampled every 50ms using SysTick interrupts
   ADCpot = ADC0_In();  										// using 12-bit ADC built into microcontroller
	 inch = ADCpot / 8;												// based on datasheet, we want it to be 9 bits thus divide by 8
	 if(inch > 12){														// if nothing is seen
		 sensorFlag = 0;												// clear flag
	 }
	 else{																		// if an object is in front
		 sensorFlag = 1;												// set flag
	 }
}

int main(void){ 
	PLL_Init();															  // Bus Clock at 80Mhz
	ADC0_Init();															// 12 bit ADC
	PortA_Init();														  // Controls ~EN and Dir for some motors
	PortB_Init(); 													  // Controls Step and Dir
 	PortD_Init(); 													  // Controls Step, Dir, and ~EN
	UART1_Init();															// Initializes UART1
	PortF_Init();															// Onboard LED's
	EnableInterrupts();												// for interrupts
	SysTick_Init(4000000);										// for sensor ADC sampling
	while(1){
			char InStringg[4];  									// stores incoming string containing time (in seconds) information
			char TimeInt[3];											// stores just the string containing the time number (basically copy of InString without 'f')
			GPIO_PORTF_DATA_R = 0x02; 					  // Turn on RED LED
			GPIO_PORTA_DATA_R = 0x3C; 					  // motors in this port off
			GPIO_PORTB_DATA_R = 0x12; 					  // motors in this port off		
			GPIO_PORTD_DATA_R = 0x80; 					  // motors in this port off
			UART1_InString(InStringg,4);					// takes in string up to 4 char. Stops reading when ENTER (\r) is found
			for(j=0;j<3;j++)										  // iterate through string array InString to get separate numbers ex: (['f', '2', '6', '2'])
			{
				TimeInt[j] = InStringg[j+1]; 				// copy numbers to new char array ex: (['2','6',2'])
			}
			sec = atoi(TimeInt);									// convert string to integer --> from "262" to int 262
			rot = sec / 2.5; 	 										// how many rotations it will take --> 2.5 depends on the overall wait time between step transition 1->0
			rotDeg = 360 * rot; 									// how many total degrees equal the rotation
			degStep = rotDeg / 1.8;								// how many steps will it take to complete it --> motor moves 1.8 degrees per step (based on datasheet)			
			if (InStringg[0] == 'f'){						
				GPIO_PORTF_DATA_R = 0x08; 								 // Turn on GREEN LED indicating process starting
				//*********************************Fridge portion**********************************************************************			
				/*
				while(sensorFlag == 0){										 // While no object is detected
					degreeSpin('B',0x11,0x10,490,80000);  	 // turn motor 3 on, last around __ seconds doing 1080 degrees giving it time to send it in front of the sensor	
				}
				sensorFlag = 0;														 // reset flag	
				*/
				degreeSpin('B',0x11,0x10,490,80000);			// turn motor 3 on --> send bowl to the end of fridge
				GPIO_PORTB_DATA_R = 0x12; 	  		    		 // stop "all motors" --> only motor 3 was turned on thus turn it off
				Delay(6);																   // .21 seconds * 6
				degreeSpin('B',0x06,0x02,100,80000);    	 // move motor 4 180 degrees (open door)				
				Delay(6);																	 // .21 seconds * 6		
				degreeSpin('B',0x01,0x00,400,80000);  		 // turn motor 3 on, 720 degrees, move motor 3 forward to send food out the door (motor 4 also on to lock door)
				GPIO_PORTB_DATA_R = 0x02; 	  		    		 // stop motor 3, leave motor 4 on						
				Delay(6);																	 // .21 seconds * 6							 
				GPIO_PORTA_DATA_R = 0x38;									 // turn on motor 5
				degreeSpin('B',0x22,0x02,315,99999);   	   // move "motor 5" forward to send to cooker (conveyor belt), leave motor 4 on
				GPIO_PORTA_DATA_R = 0x3C;									 // turn off motor 5
				Delay(6);																	 // .21 seconds * 6
				degreeSpin('B',0x0E,0x0A,100,80000);    	 // turn motor 4 on, move motor 4 180 degrees back (close door)
				GPIO_PORTB_DATA_R = 0x12; 	  		    		 // stop "all motors" --> motor 4 off
				Delay(6);																	 // .21 seconds * 6				
				
				//*********************************Cooker portion**********************************************************************				
				GPIO_PORTA_DATA_R = 0xB4; 								 // Turn on motor 6
				degreeSpin('B',0x52,0x12,150,80000);   	 	 // motor 6 slowly grips bowl turning 270 degrees (grab claw)
				Delay(3);																 // .21 secods *3
				GPIO_PORTA_DATA_R = 0x2C;									 // turn on motor 7	
				degreeSpin('D',0x83,0x82,8500,45000); 		 // move motor 7 360 deg x42.5 back (drop ingredients)
				Delay(10);																 // .21 seconds * 10
				GPIO_PORTA_DATA_R = 0x6C;									 // motor 7 other direction
				degreeSpin('D',0x81,0x80,8500,45000); 		 // move motor 7, rotate 360 deg x42.5 (have bowl to original position and not upsidedown)
				GPIO_PORTA_DATA_R = 0x3C; 								 // turn off motor 7, other motors should be off		
				Delay(10);																 // .21 secods *10
				GPIO_PORTA_DATA_R = 0x34; 								 // Turn off motor 5 and turn on motor 6
				degreeSpin('B',0x52,0x12,150,80000);		 	 // move motor 6 270 degrees back (claw lets go of the bowl)				
				GPIO_PORTA_DATA_R = 0x3C;									 // turn off motor 6, other motors should be off
				Delay(15);																 // .21 seconds * 15
				GPIO_PORTA_DATA_R = 0x1C;									 // turn on motor 8
				degreeSpin('D',0x8C,0x88,800,45000);  		 // motor 8 slowly turns 360 degrees x4 (pot tilts food)
				GPIO_PORTA_DATA_R = 0x3C; 								 // turn off motor 8, other motors should be off
				Delay(20);																 // .21 seconds * 20
				degreeSpin('D',0x40,0x00,degStep,500000);  // turns on motor 9 and turns 360 degrees continuously for certain amount of time (cooks food)
				GPIO_PORTD_DATA_R = 0x80;									 // turn off motor 9, other motors should be off
				Delay(20);																 // .21 seconds * 20
				GPIO_PORTA_DATA_R = 0x1C;									 // turn on motor 8
				degreeSpin('D',0x8C,0x88,4800,45000);      // motor 8 slowly turns 360 degrees x24 (dropping finished food on new bowl)
				GPIO_PORTA_DATA_R = 0x3C; 								 // turn off motor 8, other motors should be off
				Delay(20);																 // .21 seconds * 20
				GPIO_PORTA_DATA_R = 0x1C;									 // turn on motor 8
				degreeSpin('D',0x84,0x80,5600,45000);   	 // "motor 8" slowly turns 360 degrees x28 back (original position)
				GPIO_PORTA_DATA_R = 0x3C; 								 // turn off motor 8, other motors should be off	
			}
	}
}
