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
#include "../inc/ST7735.c"
#include "../inc/ST7735.h"
#include "../inc/LCDDriver.c"
#include "../inc/SoundDriver.c"
#include "../inc/SwitchDriver.c"



#define PF2     (*((volatile uint32_t *)0x40025010))
#define PF3     (*((volatile uint32_t *)0x40025020))
#define SETHOUR 1
#define SETMINUTE 2
#define SETAMPM 3
#define SETMILITARY 4
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
volatile uint32_t Counts = 0;
volatile uint32_t Hours = 0;
volatile uint32_t Minutes = 0;
volatile uint32_t Seconds = 0;
volatile char* AmPm = "AM";
volatile uint16_t Am = 1;
volatile uint32_t AlarmHour;
volatile uint32_t	AlarmMinute;
volatile char*	AlarmAmPm;
volatile uint16_t AlarmSet = 0;
int main(void){
	//This is for the Systick Intitialization
  PLL_Init(Bus80MHz);         // bus clock at 80 MHz
  SYSCTL_RCGCGPIO_R |= 0x20;  // activate port F
  Counts = 0;
  GPIO_PORTF_DIR_R |= 0x0C;   // make PF2,PF3 output (PF2,PF3 built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x0C;// disable alt funct on PF2,PF3
  GPIO_PORTF_DEN_R |= 0x0C;   // enable digital I/O on PF2,PF3
                              // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFF00FF)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;     // disable analog functionality on PF,PF3
  SysTick_Init(8000000);        // initialize SysTick timer
  EnableInterrupts();

	initLCD();
	DrawTime(Hours, Minutes, (char*) AmPm, AlarmSet, AlarmHour, AlarmMinute, (char*) AlarmAmPm);
	
  while(1){                   // interrupts every 1ms, 500 Hz flash
    //PF3 ^= 0x08;              // toggle PF3
  }
}

void setTime(uint32_t Hr, uint32_t Minute, char* AMPM){
	Hours = Hr;
	Minutes = Minute;
	AmPm = AMPM;
}

void setAlarm(uint32_t Hr, uint32_t Minute, char* AMPM){
	AlarmHour = Hr;
	AlarmMinute = Minute;
	AlarmAmPm = AMPM;
	AlarmSet = 1;
}

void disableAlarm(void){
	AlarmSet = 0;
}

// Interrupt service routine
// Executed every 12.5ns*(period)
void SysTick_Handler(void){
  //PF2 ^= 0x04;                // toggle PF2
  //PF2 ^= 0x04;                // toggle PF2
  Counts = Counts + 1;
  //PF2 ^= 0x04;                // toggle PF2
	if( Counts >= 10){
		Counts = 0;
		Seconds++;
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
		DrawTime(Hours, Minutes, (char*) AmPm, AlarmSet, AlarmHour, AlarmMinute, (char*) AlarmAmPm);
	}
	if( Hours >= 12){
		Hours = 0;
		if(Am == 1){
			AmPm = "PM";
			Am = 0;
		}else {
			AmPm = "AM";
			Am = 1;
		}
	}
}






