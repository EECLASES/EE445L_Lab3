#include <stdio.h>
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/Timer2A.h"
#include "../inc/Timer2A.c"
#include "SoundDriver.h"

volatile uint32_t alarmTime = 0;
volatile uint16_t alarm = 0;
	
void PC5toggle(void){
	if(alarm == 1){
		alarmTime++;
		PD0 ^= 0x01;
		if(alarmTime >= 10000){
			alarm = 0;
			alarmTime = 0;
		}
	}
}

void Sound_Init(void){
	volatile unsigned long delay;
	SYSCTL_RCGCGPIO_R |= 0x00000008;
	delay = SYSCTL_RCGC2_R;
	GPIO_PORTD_AMSEL_R &= ~0x01;
	GPIO_PORTD_PCTL_R &= ~0x0000000F;
	GPIO_PORTD_DIR_R |= 0x01;
	GPIO_PORTD_AFSEL_R &= ~0x01;
	GPIO_PORTD_DEN_R |= 0x01;
	Timer2_Init(&PC5toggle,25000);
}

void playAlarm(void){
	alarm = 1;
}
