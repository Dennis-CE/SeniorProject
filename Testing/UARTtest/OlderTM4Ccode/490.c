// 0.Documentation Section 
// 490.c
// Raul Solorio. Geoffrey Grepo. 10/07/19
// Made 1 motor spin
// Made 2 motors spin
// Now we will attempt to communicate TM4C to Pi. Pi will send a number indicating when to stop or start the motor

// DISREGARD BOTTOM COMMENT; BOTTOM TESTING WAS FOR LAST SEMESTER BUT NEVER HAPPEND
// To make it easy on us, Pi will send character 'G' indicating Start
// Pi will send character 'R' indicating Stop
// If character recieved is 'G' then make motor spin continously -- Green LED turned on 
// If character recieved is 'R' then make motor stop until told to start again -- Red LED turned on

// INSTEAD SEND 'r' or 'f'. NOTE: Case sensitive. 

// DISREGARD BOTTOM COMMENT;
// 1-11-20 --> sent a '1' or '2' to make a single motor spin either or when pressed on realterm, 'a' to make all spin, 'r' to stop all.	
// **NOTE: To change direction you have to change the dir on both when it steps on 1 and 0 **
// Python script did the above performance, however we have to run the code every time we send 1,2,a, and r.
// **FIX: Sometimes when running python script it skills over all other code ex: 1,2,r but does the first one ex: a**
// next step is to learn how to do 1it in C++
// python script was just to know we configured UART correctly

// HAVE PYTHON SCRIP SEND 'f' and 'r' constantly with certain time intervals just for testing. 

// UART 1 on PC4/PC5 --> PC4 = RX ; PC5 = TX;

#include "SysTick.h"				 // Delays
#include "PLL.h" 						 // in case we need to overclock
#include "UART.h"						 // UART 1
#include "tm4c123gh6pm.h"		 // contains all TM4C registers
#include <stdlib.h>					 // for atoi

unsigned long volatile x, i; // used in the for loop
unsigned char commandMotor;	 // UART character
unsigned int sec, rot, rotDeg, degStep, j; // time to cook


void EnableInterrupts(void);
void WaitForInterrupt(void);
void PortF_Init(void);
void PortB_Init(void);
void PortD_Init(void);
void PortE_Init(void);
void Delay(unsigned int loops);
void degreeSpin(unsigned char ports, unsigned long volatile motor, unsigned long volatile motor1, unsigned long volatile steps, unsigned long volatile time);

// PA2 = NOT ENABLE --> motor 8 
// PA3 = NOT ENABLE --> motor 9
// PA4 = NOT ENABLE --> motor 10
// PA5 = NOT ENABLE --> motor 11
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

// PB0 = Step	--> motor 5
// PB1 = NOT ENABLE  --> motor 5
// PB2 = Step	--> motor 6
// PB3 = Step
// PB4 = Dir	--> motor 7
// PB5 = Step --> motor 8
// PB6 = Step
// PB7 = Dir	--> motor 9
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

// PD2 = NOT E0NABLE --> motor 6
// PD3 = NOT ENABLE --> motor 7
void PortD_Init(void){unsigned long volatile delay;
  SYSCTL_RCGC2_R |= 0x00000008;     // 1) D clock
	delay = SYSCTL_RCGC2_R;						// delay
  GPIO_PORTD_CR_R |= 0xFF;          // allow changes to PD7-PD0         
	GPIO_PORTD_AMSEL_R &= ~0xFF;      // 3) disable analog function for PD7-PD0
	GPIO_PORTD_PCTL_R |= 0x00000000;  // Configure PD7-PD0 to GPIO
  GPIO_PORTD_DIR_R |= 0xFF;         // 5) PD7-PD0 output
  GPIO_PORTD_AFSEL_R |= 0x00;       // Select regular I/O on PD7-PD0
  GPIO_PORTD_DEN_R |= 0xFF;         // 7) enable digital pins PD7-PD0  
}

// PE0 = Step
// PE1 = Dir	--> Motor 10
// PE2 = Step
// PE3 = Dir	--> Motor 11
// PE4 = Step 
// PE5 = NOT ENABLE	--> Motor 12
void PortE_Init(void){unsigned long volatile delay;
  SYSCTL_RCGC2_R |= 0x00000010;     // 1) E clock
	delay = SYSCTL_RCGC2_R;						// delay
  GPIO_PORTE_CR_R |= 0x3F;          // allow changes to PE5-PE0         
	GPIO_PORTE_AMSEL_R &= ~0x3F;      // 3) disable analog function for PE5-PE0
	GPIO_PORTE_PCTL_R |= 0x00000000;  // Configure PE5-PE0 to GPIO
  GPIO_PORTE_DIR_R |= 0x3F;         // 5) PE5-PE0 output
  GPIO_PORTE_AFSEL_R |= 0x00;       // Select regular I/O on PE5-PE0
  GPIO_PORTE_DEN_R |= 0x3F;         // 7) enable digital pins PE5-PE0  
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
					SysTick_Wait(time);								// waits a certain amount of time. Ex:1000 microseconds (1000*10^-6)  --> (1000*10^-6) / (1/80Mhz) = 80000
					GPIO_PORTB_DATA_R = motor1;				// motors goes same way. Dir = 0 (or 1)... Step = 0 --> needs to see a transition from 1 to 0 to step
					SysTick_Wait(time);								// Wait
				}
			}
			else if(ports == 'E'){								// motors attached to this port
				for(x=0;x<steps;x++){							  // comment the same as above
					GPIO_PORTE_DATA_R = motor;				// STEP, DIRECTION and NOT ENABLE value
					SysTick_Wait(time);								// wait
					GPIO_PORTE_DATA_R = motor1;				// Should only change STEP value
					SysTick_Wait(time);								// wait
				}				
			}			
}

void Delay(unsigned int loops){
	for(i=0;i<loops;i++){										  // wait this much time to secure food is on conveyor belt
		SysTick_Wait(16777203);	  			  		  // wait .21 second --> did trial and error
	}
}

int main(void){ 
	PLL_Init();															  // Bus Clock at 80Mhz
	PortA_Init();														  // Controls ~EN for some motors
	PortB_Init(); 													  // Controls Step and Dir
 	PortD_Init(); 													  // Controls Step and Dir
	PortE_Init(); 													  // Controls Step and Dir
	UART1_Init();															// Initializes UART1
	PortF_Init();															// Onboard LED's
	EnableInterrupts();												// for interrupts
	SysTick_Init();														// using 80Mhz clock for delay
	while(1){
			char InStringg[3];  									// stores incoming string containing time (in seconds) information
			char TimeInt[2];											// stores just the string containing the time number (basically copy of InString without 'f')
			GPIO_PORTF_DATA_R = 0x02; 					  // Turn on RED LED
			GPIO_PORTA_DATA_R = 0x3C; 					  // motors in this port off
			GPIO_PORTD_DATA_R = 0x0C; 					  // motors in this port off
			GPIO_PORTE_DATA_R = 0x20; 					  // motors in this port off
			GPIO_PORTB_DATA_R = 0x02; 					  // motors in this port off	
			UART1_InString(InStringg,3);					// takes in string up to 5 char
			for(j=0;j<2;j++)										  // iterate through string array InString to get separate numbers ex: (['f', '2', '6', '2'])
			{
				TimeInt[j] = InStringg[j+1]; 				// copy numbers to new char array ex: (['2','6',2'])
			}
			sec = atoi(TimeInt);									// convert string to integer
			rot = sec / 0.5; 	 										// how many rotations it will take --> 0.5 depends on the overall wait time between step transition 1->0
			rotDeg = 360 * rot; 									// how many total degrees equal the rotation
			degStep = rotDeg / 1.8;								// how many steps will it take to complete it --> motor moves 1.8 degrees per step			
			UART1_OutString(InStringg);
			
			if (InStringg[0] == 'f'){						
				GPIO_PORTF_DATA_R = 0x08; 					// Turn on GREEN LED indicating motors on
				//*********************************Fridge portion**********************************************************************			
				//GPIO_PORTA_DATA_R = 0x3C; 
				//GPIO_PORTD_DATA_R = 0x0C;
				GPIO_PORTE_DATA_R = 0x20;									 // All these motors on these ports will turn motor NOT ENABLE active thus motor off except motor 5		--> Hmm maybe these 3 lines could be deleted since its already at this point when it's at 'r'
				degreeSpin('B',0x01,0x00,600,300000);  		 // 2700 degrees, motors 5 last around ... seconds giving it time to put food on next conveyor belt  ** CORRECT CODE FOR BELT
				GPIO_PORTB_DATA_R = 0x02;									 // previously other motors are off thus motor 5 is the only one we need to turn off, "all motors off"
				Delay(2);																	 // .21 seconds * 2?				
				GPIO_PORTD_DATA_R = 0x08;									 // turn motor 6 on
				degreeSpin('B',0x06,0x02,7200,80000);  	   // last around 14 seconds doing 12960 degrees giving it time to send it in front of the sensor "motor 6" --> note: motor 5 is on this port so turn it off
				GPIO_PORTD_DATA_R = 0x0C; 	  		    		 // stop "all motors" --> only motor 6 was turned on thus turn it off
				Delay(2);																	 // .21 seconds * 2?
				GPIO_PORTD_DATA_R = 0x04;									 // turn motor 7 on
				degreeSpin('B',0x09,0x02,50,80000);    		 // move motor 7 90 degrees (open door) and turn off motor 5
				GPIO_PORTD_DATA_R = 0x08;  							   // turn off motor 7 and turn on motor 6
				degreeSpin('B',0x06,0x02,1500,80000);  		 // 2700 degrees, move motor 6 forward to send food out the door and turn off motor 5
				GPIO_PORTD_DATA_R = 0x04;									 // turn motor 7 on and motor 6 off
				degreeSpin('B',0x1A,0x12,50,80000);    		 // move motor 7 90 degrees back (close door) and turn off motor 5				
				GPIO_PORTD_DATA_R = 0x0C;									 // Turn off motor 7, other motors should be off
				
				//*********************************Cooker portion**********************************************************************
				GPIO_PORTA_DATA_R = 0x38;									 // turn on motor 8
				degreeSpin('B',0x22,0x02,7200,80000);   	 // move "motor 8" forward to send to cooker	
				GPIO_PORTA_DATA_R = 0x34; 								 // Turn off motor 8 and turn on motor 9
				degreeSpin('B',0x42,0x02,200,80000);   	 	 // motor 9 slowly grips bowl turning 360 degrees
				GPIO_PORTA_DATA_R = 0x3C;									 // turn off motor 9, other motors should be off
				Delay(3);																	 // .21 seconds * 3?
				GPIO_PORTA_DATA_R = 0x2C;									 // turn on motor 10
				degreeSpin('E',0x21,0x20,100,1000000); 		 // move motor 10 slowly 180 degrees (drop bowl), turn off motor 12
				GPIO_PORTA_DATA_R = 0x3C; 								 // turn off motor 10, other motors should be off
				Delay(3);																	 // .21 seconds * 3?
				GPIO_PORTA_DATA_R = 0x2C;									 // turn on motor 10
				degreeSpin('E',0x23,0x22,100,1000000); 		 // move motor 10 180 degrees back (have bowl to original position and not upsidedown), turn off motor 12
				GPIO_PORTA_DATA_R = 0x3C; 								 // turn off motor 10, other motors should be off		
				GPIO_PORTA_DATA_R = 0x34; 								 // Turn on motor 9
				degreeSpin('B',0xC2,0x82,200,80000);		 	 // move motor 9 360 degrees back (claw lets go of the bowl), turn off motor 12
				GPIO_PORTA_DATA_R = 0x3C;									 // turn off motor 9, other motors should be off
				GPIO_PORTA_DATA_R = 0x1C;									 // turn on motor 11
				degreeSpin('E',0x24,0x20,25,1000000);  		 // motor 11 slowly turns 45 degrees (tilts food), turn off motor 12
				GPIO_PORTA_DATA_R = 0x3C; 								 // turn off motor 11, other motors should be off
				Delay(3);																	 // .21 seconds * 3?
				degreeSpin('E',0x10,0x00,degStep,100000);  // turns on motor 12 and turns 36000 degrees for certain amount of time (cooks food)
				GPIO_PORTE_DATA_R = 0x20;									 // turn off motor 12, other motors should be off
				Delay(3);																	 // .21 seconds * 3?
				GPIO_PORTA_DATA_R = 0x1C;									 // turn on motor 11
				degreeSpin('E',0x24,0x20,75,1000000);      // motor 11 slowly turns 135 degrees (dropping finished food on new bowl), turn off motor 12
				GPIO_PORTA_DATA_R = 0x3C; 								 // turn off motor 11, other motors should be off
				Delay(3);																	 // .21 seconds * 3?
				GPIO_PORTA_DATA_R = 0x1C;									 // turn on motor 11
				degreeSpin('E',0x2C,0x28,100,1000000);   	 // "motor 11" slowly turns 180 degrees back to original position, turn off motor 12
				GPIO_PORTA_DATA_R = 0x3C; 								 // turn off motor 11, other motors should be off		
				/*				
					while sensor != 1 --> sensor doesn't detect anything but there is a food processing
						degreeSpin(0x14, 0x04, 7200);// last around 14 seonds???  turn motor 6 on
	
					if sensor == 1
						stop motor 6
						move motor 7 90 degrees
						motor 7 stops
						degreeSpin(0x14, 0x04, 7200);// last around 14 seonds??? // move motor 6
						all wait for a certain amount of time
						move motor 7 90 degrees back
					
					--> SENSOR #2 may not be necessary ; we could just time it to stop it
				*/
			}
	}
}
