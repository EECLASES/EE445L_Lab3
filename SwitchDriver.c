// Switch.c
// Runs on TMC4C123
// Use GPIO in edge time mode to request interrupts on any
// edge of PF4 and start Timer0B. In Timer0B one-shot
// interrupts, record the state of the switch once it has stopped
// bouncing.
// Daniel Valvano
// May 3, 2015

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015

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

// PF4 connected to a negative logic switch using internal pull-up (trigger on both edges)
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#define PF4                     (*((volatile uint32_t *)0x40025040))
#define PF0                     (*((volatile uint32_t *)0x40025004))
#define PB1 (*((volatile uint32_t *)0x40005008))
#define PB2 (*((volatile uint32_t *)0x40005010))
#define PB3 (*((volatile uint32_t *)0x40005020))
#define PB4 (*((volatile uint32_t *)0x40005040))

#define GPIO_LOCK_KEY           0x4C4F434B  // Unlocks the GPIO_CR register

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

volatile static unsigned long Touch;     // true on touch
volatile static unsigned long Release;   // true on release
volatile static unsigned long Last;      // previous
void (*TouchTask)(void);    // user function to be executed on touch
void (*ReleaseTask)(void);  // user function to be executed on release

volatile static unsigned long Touch1;     // true on touch
volatile static unsigned long Release1;   // true on release
volatile static unsigned long Last1;      // previous
void (*TouchTask1)(void);    // user function to be executed on touch
void (*ReleaseTask1)(void);  // user function to be executed on release




static void Timer0Arm(void){
  TIMER0_CTL_R = 0x00000000;    // 1) disable TIMER0A during setup
  TIMER0_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER0_TAMR_R = 0x0000001;    // 3) 1-SHOT mode
  TIMER0_TAILR_R = 160000;      // 4) 10ms reload value
  TIMER0_TAPR_R = 0;            // 5) bus clock resolution
  TIMER0_ICR_R = 0x00000001;    // 6) clear TIMER0A timeout flag
  TIMER0_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 35, interrupt number 19
  NVIC_EN0_R = 1<<19;           // 9) enable IRQ 19 in NVIC
  TIMER0_CTL_R = 0x00000001;    // 10) enable TIMER0A
}
static void GPIOArm(void){
  GPIO_PORTF_ICR_R = 0x11;      // (e) clear flag4 and flag0
  GPIO_PORTF_IM_R |= 0x11;      // (f) arm interrupt on PF4 *** No IME bit as mentioned in Book ***
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
  NVIC_EN0_R = 0x40000000;      // (h) enable interrupt 30 in NVIC  
}
// Initialize switch interface on PF4 and pf0
// Inputs:  pointer to a function to call on touch (falling edge),
//          pointer to a function to call on release (rising edge)
// Outputs: none 
void Switch_Init(void(*touchtask)(void), void(*releasetask)(void)){
  // **** general initialization ****
  SYSCTL_RCGCGPIO_R |= 0x00000020; // (a) activate clock for port F
  while((SYSCTL_PRGPIO_R & 0x00000020) == 0){};
   GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
	GPIO_PORTF_CR_R |= 0x11;  // 2b) enable commit for PF4 and PF0

	GPIO_PORTF_DIR_R &= ~0x11;    // (c) make PF4 in (built-in button)
  GPIO_PORTF_AFSEL_R &= ~0x11;  //     disable alt funct on PF4
  GPIO_PORTF_DEN_R |= 0x11;     //     enable digital I/O on PF4   
  GPIO_PORTF_PCTL_R &= ~0x000F0001; // configure PF4 as GPIO
  GPIO_PORTF_AMSEL_R = 0;       //     disable analog functionality on PF
  GPIO_PORTF_PUR_R |= 0x11;     //     enable weak pull-up on PF4
  GPIO_PORTF_IS_R &= ~0x11;     // (d) PF4 is edge-sensitive
  GPIO_PORTF_IBE_R |= 0x11;     //     PF4 is both edges
  GPIOArm();

  SYSCTL_RCGCTIMER_R |= 0x01;   // 0) activate TIMER0
  TouchTask = touchtask;           // user function 
  ReleaseTask = releasetask;       // user function 
  Touch = 0;                       // allow time to finish activating
  Release = 0;
  Last = ((PF4|PF0)&0x11);                      // initial switch state
 }
// Interrupt on rising or falling edge of PF4 (CCP0)
void GPIOPortF_Handler(void){
  GPIO_PORTF_IM_R &= ~0x11;     // disarm interrupt on PF4 
  
if(Last==0x10 || Last==0x01){    // 0x10 means it was previously released
    Touch = 1;       // touch occurred
    (*TouchTask)();  // execute user task
  }
  else if (Last!=0x10 || Last!=0x01){
    Release = 1;       // release occurred
    (*ReleaseTask)();  // execute user task
  }
  Timer0Arm(); // start one shot
}
// Interrupt 10 ms after rising edge of PF4
void Timer0A_Handler(void){
  TIMER0_IMR_R = 0x00000000;    // disarm timeout interrupt
  Last = ((PF4|PF0)&0x11);;  // switch state
  GPIOArm();   // start GPIO
}

// Wait for switch to be pressed 
// There will be minimum time delay from touch to when this function returns
// Inputs:  none
// Outputs: none 
void Switch_WaitPress(void){
  while(Touch==0){}; // wait for press
  Touch = 0;  // set up for next time
}

// Wait for switch to be released 
// There will be minimum time delay from release to when this function returns
// Inputs:  none
// Outputs: none 
void Switch_WaitRelease(void){
  while(Release==0){}; // wait
  Release = 0; // set up for next time
}

// Return current value of the switch 
// Repeated calls to this function may bounce during touch and release events
// If you need to wait for the switch, use WaitPress or WaitRelease
// Inputs:  none
// Outputs: false if switch currently pressed, true if released 
unsigned long Switch_Input(void){
  return ((PF4|PF0)&0x11);
}

















//This is the Switch init for portb functions
static void Timer1Arm(void){
  TIMER1_CTL_R = 0x00000000;    // 1) disable TIMER0A during setup
  TIMER1_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER1_TAMR_R = 0x0000001;    // 3) 1-SHOT mode
  TIMER1_TAILR_R = 160000;      // 4) 10ms reload value
  TIMER1_TAPR_R = 0;            // 5) bus clock resolution
  TIMER1_ICR_R = 0x00000001;    // 6) clear TIMER0A timeout flag
  TIMER1_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 35, interrupt number 19
  NVIC_EN0_R = 1<<21;           // 9) enable IRQ 19 in NVIC
  TIMER1_CTL_R = 0x00000001;    // 10) enable TIMER0A
}
static void GPIOArm1(void){
  GPIO_PORTB_ICR_R = 0x1E;      // (e) clear flag4 and flag0
  GPIO_PORTB_IM_R |= 0x1E;      // (f) arm interrupt on PF4 *** No IME bit as mentioned in Book ***
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
  NVIC_EN0_R = 0x00000001;      // (h) enable interrupt 1 in NVIC  
}
void Timer1A_Handler(void){
  TIMER1_IMR_R = 0x00000000;    // disarm timeout interrupt
  Last1 = ((PB1|PB2|PB3|PB4)&0x1E);  // switch state
  GPIOArm1();   // start GPIO
}
void Switch_Init1(void(*touchtask1)(void), void(*releasetask1)(void)){
  // **** general initialization ****
  SYSCTL_RCGCGPIO_R |= 0x00000002; // (a) activate clock for port F
  while((SYSCTL_PRGPIO_R & 0x00000002) == 0){};
   
	GPIO_PORTB_DIR_R &= ~0x1E;    // (c) make PF4 in (built-in button)
  GPIO_PORTB_AFSEL_R &= ~0x1E;  //     disable alt funct on PF4
  GPIO_PORTB_DEN_R |= 0x1E;     //     enable digital I/O on PF4   
  GPIO_PORTB_PCTL_R &= ~0x000FFFF0; // configure PF4 as GPIO
  GPIO_PORTB_AMSEL_R = 0;       //     disable analog functionality on PF
  //GPIO_PORTB_PUR_R |= 0x1E;     //     enable weak pull-up on PF4
  GPIO_PORTB_IS_R &= ~0x1E;     // (d) PF4 is edge-sensitive
  GPIO_PORTB_IBE_R &= ~0x1E;     //     PF4 is both edges
  GPIO_PORTB_IEV_R |= 0x1E;
		
		GPIOArm1();

  SYSCTL_RCGCTIMER_R |= 0x02;   // 0) activate TIMER0//
  TouchTask1 = touchtask1;           // user function 
  ReleaseTask1 = releasetask1;       // user function 
  Touch1 = 0;                       // allow time to finish activating
  Release1 = 0;
  Last1 = ((PB1|PB2|PB3|PB4)&0x1E);                      // initial switch state
 }
// Interrupt on rising or falling edge of PF4 (CCP0)
void GPIOPortB_Handler(void){
  GPIO_PORTB_IM_R &= ~0x1E;     // disarm interrupt on PF4 
  if(Last1){    // 0x10 means it was previously released
    Touch1 = 1;       // touch occurred
    (*TouchTask1)();  // execute user task
  }
  else {
    Release1 = 1;       // release occurred
    (*ReleaseTask1)();  // execute user task
  }
  Timer1Arm(); // start one shot
}



//let's try to configure portc to work with the interrupts


volatile uint32_t FallingEdges = 0;
void EdgeCounter_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x04;  //activate clock for port c
	FallingEdges = 0;
	GPIO_PORTC_DIR_R &= ~0xF0;  //make portc an input
	GPIO_PORTC_DEN_R |=  0xF0;
	GPIO_PORTC_IS_R &=  ~0xF0;  //PORTC IS EDGE SENSITIVE
	GPIO_PORTC_IBE_R &= ~0xF0;
	GPIO_PORTC_IEV_R |= 0xF0;   //BC FALLING EDGE
	GPIO_PORTC_ICR_R = 0xF0;   //CLEAR FLAG 4ON PC4
	GPIO_PORTC_IM_R |= 0xF0;   //CLEAR FLAG 4ON PC4
  NVIC_PRI0_R = (NVIC_PRI0_R&0xFF00FFFF)|0x00A00000; //PRIORITY 5
	NVIC_EN0_R = 4; //ENABLE INTERRUPT TWO
	
	
	
}



