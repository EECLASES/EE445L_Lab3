//This is the SwitchDriver file with Function Definitions

#include "SwitchDriver.h"

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
void Switch_Init(void(*touchtask)(void), void(*releasetask)(void),
										void(*touchtask1)(void),void(*releasetask1)(void)){
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
	TouchTask1 = touchtask1;
	ReleaseTask1 = releasetask1;
  Touch = 0;                       // allow time to finish activating
  Release = 0;
  Last = ((PF4|PF0)&0x11);                      // initial switch state
 }
// Interrupt on rising or falling edge of PF4 (CCP0)
void GPIOPortF_Handler(void){
  GPIO_PORTF_IM_R &= ~0x11;     // disarm interrupt on PF4 
  
if(Last==0x10 ){    // 0x10 means it was previously released
    Touch = 1;       // touch occurred
    (*TouchTask)();  // execute user task
  }
  else if (Last!=0x10 || Last!=0x01){
    Release = 1;       // release occurred
    (*ReleaseTask)();  // execute user task
  }
	
	if(Last==0x01 ){    // 0x10 means it was previously released
    Touch1 = 1;       // touch occurred
    (*TouchTask1)();  // execute user task
  }
	else if (Last!=0x01){
    Release1 = 1;       // release occurred
    (*ReleaseTask1)();  // execute user task
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






















 
 
static void GPIOArm1(void){ 
	GPIO_PORTC_ICR_R = 0xF0;   //CLEAR FLAG 4ON PC4
	GPIO_PORTC_IM_R |= 0xF0;   //CLEAR FLAG 4ON PC4
  NVIC_PRI0_R = (NVIC_PRI0_R&0xFF00FFFF)|0x00A00000; //PRIORITY 5
	NVIC_EN0_R = 4; //ENABLE INTERRUPT TWO
	
}
void Timer1A_Handler(void){
  TIMER1_IMR_R = 0x00000000;    // disarm timeout interrupt
  Last1 = ((PC4|PC5|PC6|PC7)&0xF0);  // switch state
  GPIOArm1();   // start GPIO
}

static void Timer1Arm(void){
  TIMER1_CTL_R = 0x00000000;    // 1) disable TIMER0A during setup
  TIMER1_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER1_TAMR_R = 0x0000001;    // 3) 1-SHOT mode
  TIMER1_TAILR_R = 160000;      // 4) 10ms reload value
  TIMER1_TAPR_R = 0;            // 5) bus clock resolution
  TIMER1_ICR_R = 0x00000001;    // 6) clear TIMER0A timeout flag
  TIMER1_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF00FF)|0x00008000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 35, interrupt number 19
  NVIC_EN0_R = 1<<21;           // 9) enable IRQ 19 in NVIC
  TIMER1_CTL_R = 0x00000001;    // 10) enable TIMER1A
}
//let's try to configure portc to work with the interrupts


volatile uint32_t FallingEdges = 0;
void EdgeCounter_Init(void(*touchtask2)(void), void(*releasetask2)(void),
										void(*touchtask3)(void),void(*releasetask3)(void),
											void(*touchtask4)(void), void(*releasetask4)(void),
										void(*touchtask5)(void),void(*releasetask5)(void)){
	SYSCTL_RCGCGPIO_R |= 0x04;  //activate clock for port c
	FallingEdges = 0;
	GPIO_PORTC_DIR_R &= ~0xF0;  //make portc an input
	GPIO_PORTC_DEN_R |=  0xF0;
	GPIO_PORTC_IS_R &=  ~0xF0;  //PORTC IS EDGE SENSITIVE
	GPIO_PORTC_IBE_R &= ~0xF0;
	GPIO_PORTC_IEV_R |= 0xF0;   //BC FALLING EDGE
	
	GPIOArm1();
	

											
	SYSCTL_RCGCTIMER_R |= 0x02;   // 0) activate TIMER0//
											
	//C4
  TouchTask2 = touchtask2;           // user function 
  ReleaseTask2 = releasetask2;       // user function 
  Touch2 = 0;                       // allow time to finish activating
  Release2 = 0;
 
	//C5
	TouchTask3 = touchtask3;           // user function 
  ReleaseTask3 = releasetask3;       // user function 
  Touch3= 0;                       // allow time to finish activating
  Release3= 0;								
											
	//C6
	TouchTask4 = touchtask4;           // user function 
  ReleaseTask4 = releasetask4;       // user function 
  Touch4= 0;                       // allow time to finish activating
  Release4= 0;
	
	//C7
	TouchTask5 = touchtask5;           // user function 
  ReleaseTask5 = releasetask5;       // user function 
  Touch5 = 0;                       // allow time to finish activating
  Release5 = 0;
											
	Last1 = ((PC4|PC5|PC6|PC7)&0xF0);                      // initial switch state

}

void GPIOPortC_Handler(void){
	  GPIO_PORTC_IM_R &= ~0xF0;     // disarm interrupt on PF4 

	if(Last1==0x10 ){    // 0x10 means it was previously released
    Touch2 = 1;       // touch occurred
    (*TouchTask2)();  // execute user task
			
			if (Last1!=0x10 ){
				Release2 = 1;       // release occurred
				(*ReleaseTask2)();  // execute user task
			
			}
		}
	
			
	if(Last1==0x20 ){    // 0x10 means it was previously released
    Touch3 = 1;       // touch occurred
    (*TouchTask3)();  // execute user task
		
  
		if (Last1!=0x20){
			Release3 = 1;       // release occurred
			(*ReleaseTask3)();  // execute user task
		}
	}
			
	
	else if(Last1==0x40 ){    // 0x10 means it was previously released
    Touch4= 1;       // touch occurred
    (*TouchTask4)();  // execute user task
		
  }
	else if (Last1!=0x4){
    Release4= 1;       // release occurred
    (*ReleaseTask4)();  // execute user task

  }
	Timer1Arm();
			
	if(Last1==0x80 ){    // 0x10 means it was previously released
    Touch5 = 1;       // touch occurred
    (*TouchTask5)();  // execute user task
  }
	else if (Last1!=0x80){
    Release5 = 1;       // release occurred
    (*ReleaseTask5)();  // execute user task
  }
	Timer1Arm();
			return;
}

