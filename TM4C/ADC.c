// ADC.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0 SS2 to be triggered by
// software and trigger a conversion, wait for it to finish,
// and return the result. 
// Daniel Valvano
// January 15, 2016

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2015

 Copyright 2016 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

#include "ADC.h"
#include "tm4c123gh6pm.h"

// SS2 ( Sample Sequencer) (# of samples: 4 & Depth of FIFO: 4)
// This initialization function sets up the ADC 
// Max sample rate: <=125,000 samples/second
// SS2 triggering event: software trigger
// SS2 1st sample source:  channel 0
// SS2 interrupts: enabled but not promoted to controller
void ADC0_Init(void){   
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000010;   // 1) activate clock for Port E
  delay = SYSCTL_RCGC2_R;         //    allow time for clock to stabilize
  GPIO_PORTE_DIR_R &= ~0x10;      // 2) make PE3 input
  GPIO_PORTE_AFSEL_R |= 0x10;     // 3) enable alternate function on PE3
  GPIO_PORTE_DEN_R &= ~0x10;      // 4) disable digital I/O on PE3
  GPIO_PORTE_AMSEL_R |= 0x10;     // 5) enable analog function on PE3
  SYSCTL_RCGC0_R |= 0x00010000;   // 6) activate ADC0 
  delay = SYSCTL_RCGC2_R;         
  SYSCTL_RCGC0_R &= ~0x00000300;  // 7) configure for 125K 
  ADC0_SSPRI_R = 0x3210;          // 8) Sequencer 3 is lowest priority
  ADC0_ACTSS_R &= ~0x0004;        // 9) disable sample sequencer 2
  ADC0_EMUX_R &= ~0x0F00;         // 10) seq2 is software trigger
  ADC0_SSMUX2_R = (ADC0_SSMUX2_R&0xFFFFFFF0)+9; // 11) channel Ain9 (PE4)
  ADC0_SSCTL2_R = 0x0600;         // 12) no TS3 D3, yes IE3 END3 (pg. 853 in pdf)
  ADC0_ACTSS_R |= 0x0004;         // 13) enable sample sequencer 2
}

//------------ADC0_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
unsigned long ADC0_In(void){  
  unsigned long data;
	ADC0_PSSI_R = 0x0004;							// 1) initiate SS2
	while((ADC0_RIS_R&0x04)==0){};    // 2) wait for conversion done
	data = ADC0_SSFIFO2_R&0xFFF;  		// 3) read result
	ADC0_ISC_R = 0x0004;							// 4) acknowledge completion
	return data;
}
