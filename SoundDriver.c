#include <stdio.h>
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/Timer2A.h"
#include "../inc/Timer2A.c"

#define PC5 (*((volatile unsigned long *) 0x40006080))
	
volatile uint32_t alarmTime = 0;
volatile uint16_t alarm = 0;
	
void PC5toggle(void){
	if(alarm == 1){
		alarmTime++;
		PC5 = PC5^0x20;
		if(alarmTime >= 1000){
			alarm = 0;
			alarmTime = 0;
		}
	}
}

void Sound_Init(void){
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000004;
	delay = SYSCTL_RCGC2_R;
	GPIO_PORTC_AMSEL_R &= ~0x20;
	GPIO_PORTC_PCTL_R &= ~0x00F00000;
	GPIO_PORTC_DIR_R |= 0x20;
	GPIO_PORTC_AFSEL_R &= ~0x20;
	GPIO_PORTC_DEN_R |= 0x20;
	Timer2_Init(&PC5toggle,25000);
}

void playAlarm(void){
	alarm = 1;
}
