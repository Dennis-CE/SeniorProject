// 0.Documentation Section 
// 490.c
// Raul Solorio. Geoffrey Grepo. 10/07/19
// Made 1 motor spin
// Made 2 motor spin
// Now we will attempt to communicate TM4C to Pi. Pi will send a number indicating when to stop or start the motor

// To make it easy on us, Pi will send character 'G' indicating Start
// Pi will send character 'R' indicating Stop
// If character recieved is 'G' then make motor spin continously -- Green LED turned on 
// If character recieved is 'R' then make motor stop until told to start again -- Red LED turned on

// UART 1 on PC4/PC5
#include "SysTick.h"				// Delays
#include "PLL.h" 						// in case we need to overclock
#include "UART.h"						// UART 1
#include "tm4c123gh6pm.h"		// contains all TM4C registers

unsigned long volatile x;		// used in the for loop

unsigned char commandMotor, previousCommandChar;

void EnableInterrupts(void);
void WaitForInterrupt(void);
void PortF_Init(void);
void PortB_Init(void);


// ---------------------------UART0----------------------
// ------------UART_Init------------
// Initialize the UART for 115200 baud rate (assuming 80 MHz UART clock),
// 8 bit word length, no parity bits, one stop bit, FIFOs enabled
// Input: none
// Output: none
void UART_Init(void){
  SYSCTL_RCGC1_R |= SYSCTL_RCGC1_UART0; // activate UART0
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA; // activate port A
  UART0_CTL_R &= ~UART_CTL_UARTEN;      // disable UART
  UART0_IBRD_R = 43;                    // IBRD = int(80,000,000 / (16 * 115200)) = int(43.402778)
  UART0_FBRD_R = 26;                    // FBRD = round(0.402778 * 64) = 26
                                        // 8 bit word length (no parity bits, one stop bit, FIFOs)
  UART0_LCRH_R = (UART_LCRH_WLEN_8|UART_LCRH_FEN);
  UART0_CTL_R |= UART_CTL_UARTEN;       // enable UART
  GPIO_PORTA_AFSEL_R |= 0x03;           // enable alt funct on PA1,PA0
  GPIO_PORTA_DEN_R |= 0x03;             // enable digital I/O on PA1,PA0
                                        // configure PA1,PA0 as UART0
  GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R&0xFFFFFF00)+0x00000011;
  GPIO_PORTA_AMSEL_R &= ~0x03;          // disable analog functionality on PA1,PA0
}
//------------UART_InCharNonBlocking------------
// Get oldest serial port input and return immediately
// if there is no data.
// Input: none
// Output: ASCII code for key typed or 0 if no character
unsigned char UART_InCharNonBlocking(void){
// as part of Lab 11, modify this program to use UART0 instead of UART1
  if((UART0_FR_R&UART_FR_RXFE) == 0){
    return((unsigned char)(UART0_DR_R&0xFF));
  } else{
    return 0;
  }
}
//-----------------------------------------------------------------------------


// PB4 = Step
// PB5 = Dir
// PB6 = Step
// PB7 = Dir
// 7654_3210
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

// PortF Initialization fucntion
// For Green, Blue, Red LED
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

int main(void){ 
	PLL_Init();															// Bus Clock at 80Mhz
	PortB_Init(); 													// Controls Step and Dir
	
	UART_Init();	// UART0 for testing
	
	UART1_Init();														// Initializes UART1
	PortF_Init();														// Onboard LED's
	EnableInterrupts();											// for interrupts
	SysTick_Init();													// using 80Mhz clock for delay
  previousCommandChar = 'r';
	while(1){
			commandMotor = UART_InCharNonBlocking(); // Change it to UART1_...
			
			if(commandMotor == 'g'){
				previousCommandChar = 'g';
			}
			else if(commandMotor == 'r'){
				previousCommandChar = 'r';
			}
			else{
				previousCommandChar = previousCommandChar;
			}
		
		
			if (previousCommandChar == 'g'){
					GPIO_PORTF_DATA_R = 0x08; // Turn on GREEN LED
																		// Turn on motor
					for(x=0;x<200*2;x++){									// motor steps per revolution = 200 --> Motor 1 & Motor 2
						GPIO_PORTB_DATA_R = 0x50;						// motors goes one way. Dir = 0...Step = 1
						SysTick_Wait(80000);								// wait 1000 microseconds (1000*10^-6)  --> (1000*10^-6) / (1/80Mhz) = 80000 ---> Change this to 80Mhz
						GPIO_PORTB_DATA_R = 0x00;						// motors goes same way. Dir = 0... Step = 0 --> needs to see a transition from 1 to 0 to step
						SysTick_Wait(80000);								// wait 1000 microseconds (1000*10^-6)  --> (1000*10^-6) / (1/80Mhz) = 80000 ---> Change this to 80Mhz
					}	
			}
			if(previousCommandChar == 'r'){
					GPIO_PORTF_DATA_R = 0x02; //Turn on RED LED
					//Stop motor
			}
	}
}
