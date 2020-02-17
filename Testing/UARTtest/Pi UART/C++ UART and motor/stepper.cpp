#include <iostream>
#include <wiringPi.h>				// GPIO library
#include <wiringSerial.h>			// serial library

using namespace std;

int main()
{
	int fd;
	fd = serialOpen("/dev/serial0",38400); // opens AMA0 at a baud rate of 38400, 8N1
	wiringPiSetup();		       // set up wiring pi --> mandatory; found in his website
	pinMode(0,OUTPUT); 		       // CONFIGURE GPIO17 (0 for wiringPI) as output for step
	pinMode(1,OUTPUT); 		       // GPIO18 is DIR 
	pinMode(2,OUTPUT); 		       // GPIO27 is ~EN (turn on/off motor) --> MOTOR 1
	pinMode(3,OUTPUT); 		       // GPIO22 STEP
	pinMode(4,OUTPUT);                     // GPIO23 DIR
	pinMode(5,OUTPUT);                     // GPIO24 ~EN ; MOTOR 2
	pinMode(6,OUTPUT);                     // GPIO25 STEP
	pinMode(7,OUTPUT);                     // GPIO4 DIR
	pinMode(8,OUTPUT);                     // GPIO2 ~EN; MOTOR 3		

	while(1)
	{
		int slot;
		cout << "Which slot is the food you desire? ";
		cin >> slot;
		
		if(slot == 1)
		{
			/*
				CLOCKWISE
			*/
			digitalWrite(2,0); 	   // motor on
			digitalWrite(1,0);	   // motor spins clockwise
			for(int x=0;x<50;x++) 	   // spin 90 degrees
			{
				digitalWrite(0,1); // step
				delay(1);	   // wait 1ms
				digitalWrite(0,0); // see the stepping transition (1->0)
				delay(1);	   // wait 1ms
			}
			digitalWrite(2,1); 	   // turn off motor
			delay(1000);		   // wait 1000  milliseconds
			/*
				COUNTER-CLOCKWISE
			*/			
			digitalWrite(2,0);	   // turn on motor
			digitalWrite(1,1);	   // motor spins counter-clockwise
			for(int x=0; x<50; x++)    // spin 90 dgrees
			{
				digitalWrite(0,1); // step
				delay(1);	   // wait 1ms
				digitalWrite(0,0); // see the stepping transition
				delay(1);	   // wait 1ms
			}
		        digitalWrite(2,1);	   // turn off motor
			delay(1000);		   // wait 1000 millisecond
			serialPuts(fd,"f30\r");	   // sends string "f30" via UART...TM4C needs to see 'ENTER' aka \r\n towork
		}
		else if(slot == 2)
		{
			digitalWrite(5,0);	   // motor on
			digitalWrite(4,0);	   // spin clockwise
			for(int x=0; x<50; x++)    // spin 90 degrees
			{
				digitalWrite(3,1); // wait 1ms
				delay(1);          // wait 1ms
				digitalWrite(3,0); // wait 1ms
				delay(1);	   // wait 1ms
			}
			digitalWrite(5,1);	   // turn off motor
			delay(1000);		   // wait 1000ms
			digitalWrite(5,0);	   // turn on motor
			digitalWrite(4,1);	   // motor spins counter-clockwise
			for(int x=0; x<50; x++)    // spin 90 degrees
			{
				digitalWrite(3,1); // step
				delay(1);	   // wait 1ms
				digitalWrite(3,0); // see the stepping transition
				delay(1);	   // wait 1ms
			}
		        digitalWrite(5,1);	   // turn off motor
			delay(1000);		   // wait 1000 milliseconds
			serialPuts(fd,"f30\r");	   // sends string "f30" via UART...TM4C needs to see 'ENTER' aka \r\n towork
		}
		else if(slot == 3)
		{	
			digitalWrite(8,0);	   // turn motor on
			digitalWrite(7,0);	   // turn clockwise
			for(int x=0; x<50;x++){    // 90 degrees
				digitalWrite(6,1); // step
				delay(1);	   // wait 1ms
				digitalWrite(6,0); // see the stepping transition
				delay(1);	   // wait 1ms
			}
			digitalWrite(8,1);
			delay(1000);
			digitalWrite(8,0);	   // turn on motor
			digitalWrite(7,1);	   // motor spins counter-clockwise
			for(int x=0; x<50; x++)    // spin 90 dgrees
			{
				digitalWrite(6,1); // step
				delay(1);	   // wait 1ms
				digitalWrite(6,0); // see the stepping transition
				delay(1);	   // wait 1ms
			}
		        digitalWrite(8,1);	   // turn off motor
			delay(1000);		   // wait 1000 milliseconds
			serialPuts(fd,"f30\r");	   // sends string "f30" via UART...TM4C needs to see 'ENTER' aka \r\n towork
		}
		else
		{
			digitalWrite(2,1);	   // turn off motor 
			digitalWrite(5,1);	   // turn off motor
			digitalWrite(8,1);	   // turn off motor
		}

	}
}
