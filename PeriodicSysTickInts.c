// PeriodicSysTickInts.c
// Runs on LM4F120
// Use the SysTick timer to request interrupts at a particular period.
// Daniel Valvano
// March 7, 2016

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015

   Program 5.12, section 5.7

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
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

// oscilloscope or LED connected to PF2 for period measurement
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

#include "../inc/SysTickInts.h"
#include "../inc/PLL.h"
#include "ST7735.c"
#include "../inc/ST7735.h"
#include "LCDDriver.c"
#include "SwitchDriver.c"
#include "SoundDriver.c"
//included all header files
#include "LCDDriver.h"
#include "SwitchDriver.h"
#include "SoundDriver.h"
//#include "Systick.c"
//#include "Systick.h"

#define PB0 (*((volatile uint32_t *)0x40005004))

#define PF0       (*((volatile uint32_t *)0x40025004))
#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
#define PF4       (*((volatile uint32_t *)0x40025040))
#define LEDS      (*((volatile uint32_t *)0x40025038))
#define RED       0x02
#define BLUE      0x04
#define GREEN     0x08
#define SWITCHES  (*((volatile uint32_t *)0x40025044))
#define SW1       0x10                      // on the left side of the Launchpad board
#define SW2       0x01                      // on the right side of the Launchpad board


#define PB0 (*((volatile uint32_t *)0x40005004))

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
volatile uint32_t Counts = 0;
volatile uint32_t Hours = 12;
volatile uint32_t Minutes = 0;
volatile uint32_t Seconds = 0;
volatile char* AmPm = "AM";
volatile uint16_t Am = 1;
volatile uint32_t AlarmHour = 12;
volatile uint32_t	AlarmMinute = 0;
volatile char*	AlarmAmPm = "AM";
volatile uint16_t AlarmSet = 0;
volatile uint16_t AlarmAm = 1;

volatile uint32_t SpeakerCount = 0;
int state = 0;



void incHour(){
	Hours++;
	if( Hours >= 13){
		Hours = 1;
	}
	DrawTime(Hours, Minutes, (char*) AmPm, AlarmSet, AlarmHour, AlarmMinute, (char*) AlarmAmPm);
}
void incMinute(){
	Minutes++;
	if(Minutes >= 60){
		Minutes = 0;
		Hours++;
		if( Hours >= 13){
			Hours = 1;
		}
	}
	DrawTime(Hours, Minutes, (char*) AmPm, AlarmSet, AlarmHour, AlarmMinute, (char*) AlarmAmPm);
}
void toggleAmPm(){
	if(Am == 1){
				AmPm = "PM";
				Am = 0;
			}else {
				AmPm = "AM";
				Am = 1;
			}
	DrawTime(Hours, Minutes, (char*) AmPm, AlarmSet, AlarmHour, AlarmMinute, (char*) AlarmAmPm);
}

void incAlarmHour(){
	AlarmHour++;
	if( AlarmHour >= 13){
		AlarmHour = 1;
	}
	DrawTime(Hours, Minutes, (char*) AmPm, AlarmSet, AlarmHour, AlarmMinute, (char*) AlarmAmPm);
}
void incAlarmMinute(){
	AlarmMinute++;
	if(AlarmMinute >= 60){
		AlarmMinute = 0;
		AlarmHour++;
		if( AlarmHour >= 13){
			AlarmHour = 1;
		}
	}
	DrawTime(Hours, Minutes, (char*) AmPm, AlarmSet, AlarmHour, AlarmMinute, (char*) AlarmAmPm);
}
void toggleAlarmAmPm(){
	if(AlarmAm == 1){
				AlarmAmPm = "PM";
				AlarmAm = 0;
			}else {
				AlarmAmPm = "AM";
				AlarmAm = 1;
			}
	DrawTime(Hours, Minutes, (char*) AmPm, AlarmSet, AlarmHour, AlarmMinute, (char*) AlarmAmPm);
}

void toggleAlarm(void){
	if(AlarmSet == 0){
		AlarmSet = 1;
	}else {
		AlarmSet = 0;
	}
	DrawTime(Hours, Minutes, (char*) AmPm, AlarmSet, AlarmHour, AlarmMinute, (char*) AlarmAmPm);

}
//THIS IS FOR THE SWITCH
uint32_t RiseCount,FallCount;
void Rise(void){
  
  RiseCount++;
}
void Fall(void){
  
  FallCount++;

		
	if(state ==0 ) {
		incHour();
	}
	else if (state ==1) {
		incAlarmHour();
	}
	
	
	
}
void Rise1(void){
  
  RiseCount++;
}
void Fall1(void){

		
	if(state == 0){
		incMinute();
	}
	else if (state ==1){
		incAlarmMinute();
	}

	
}

void Rise2(void){
  
  RiseCount++;
	
}
void Fall2(void){

		
	if(state ==0) {
		state = 1;
		
	}
	else if(state == 1) {
		state = 0;
		
	}
	
}

void Rise3(void){
  
}
void Fall3(void){
  
		
	if(state ==0){
		toggleAmPm();
	}
	else if(state ==1){
		toggleAlarmAmPm();
	}
	
	
}
void Rise4(void){
  
}

void Fall4(void){
	playAlarm();
			AlarmSet = 1;
	PF1 ^= 1;
			
			

  
	
}
void Rise5(void){
 
}
void Fall5(void){
  		playAlarm();

 
	
}




int main(void){
	//This is for the Systick Intitialization
  PLL_Init(Bus80MHz);         // bus clock at 80 MHz
  SYSCTL_RCGCGPIO_R |= 0x22;  // activate port F and B
  while((SYSCTL_PRGPIO_R & 0x00000020) == 0){};

	Counts = 0;
		Switch_Init(&Fall,&Rise,&Fall1,&Rise1);     // initialize GPIO Port F interrupt
		EdgeCounter_Init(&Fall2,&Rise2,&Fall3,&Rise3, &Fall4,&Rise4,&Fall5,&Rise5);
		//Switch_Init();
		//Switch_Init3();
	  //Board_Init();  
		// initialize PF0 and PF4 and make them inputs
  GPIO_PORTF_DIR_R |= 0x0E;   // make PF2,PF3,PF1 output (PF2,PF3 built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x0C;// disable alt funct on PF2,PF3 WONT BE DOING THAT
  GPIO_PORTF_DEN_R |= 0x0E;   // enable digital I/O on PF2,PF3
                              // configure PF2 as GPIO
	GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFF00FF)+0x00000000;
	GPIO_PORTF_AMSEL_R = 0;     // disable analog functionality on PF,PF3
	

	
  SysTick_Init(80000);        // initialize SysTick timer
	Sound_Init();
  EnableInterrupts();

	initLCD();
	DrawTime(Hours, Minutes, (char*) AmPm, AlarmSet, AlarmHour, AlarmMinute, (char*) AlarmAmPm);
	
  while(1){                   // interrupts every 1ms, 500 Hz flash
   
    
		
		
    //PF1 ^= 0x02;
  }
}



// Interrupt service routine
// Executed every 12.5ns*(period)
void SysTick_Handler(void){

  //PF2 ^= 0x04;                // toggle PF2
  //PF2 ^= 0x04;                // toggle PF2
  Counts = Counts + 1;
	SpeakerCount = SpeakerCount +1;
	if(SpeakerCount == 38){
		//PD0 ^= 0;
		SpeakerCount = 0;
	}
	
	
	

	if(Counts == 1000){
		Counts = 0;
		Seconds++;
		PF2 ^= 0X04;
		
		DrawTime(Hours, Minutes, (char*) AmPm, AlarmSet, AlarmHour, AlarmMinute, (char*) AlarmAmPm);
		PF2 ^= 0X04;
		PF2 ^= 0X04;
		PF3 ^= 0X08;


	}
	if( Seconds >= 60){
		Seconds = 0;
		Minutes++;
		if(Minutes < 60){
			DrawTime(Hours, Minutes, (char*) AmPm, AlarmSet, AlarmHour, AlarmMinute, (char*) AlarmAmPm);
		}
	}
	if(Minutes >= 60){
		Minutes = 0;
		Hours++;
		if(Hours == 12){
			if(Am == 1){
				AmPm = "PM";
				Am = 0;
			}else {
				AmPm = "AM";
				Am = 1;
			}
		}
		DrawTime(Hours, Minutes, (char*) AmPm, AlarmSet, AlarmHour, AlarmMinute, (char*) AlarmAmPm);
	}
	if( Hours >= 13){
		Hours = 1;
	}
	
	if((AlarmSet == 1) && (AlarmAm == Am) && (AlarmHour == Hours) && (AlarmMinute == Minutes) && (Counts == 0) && (Seconds <= 30)){
		playAlarm();
	}

	

}


