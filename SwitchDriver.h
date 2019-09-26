// PF4 connected to a negative logic switch using internal pull-up (trigger on both edges)

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

#define PF2       (*((volatile uint32_t *)0x40025010))

#define PF4                     (*((volatile uint32_t *)0x40025040))
#define PF0                     (*((volatile uint32_t *)0x40025004))
#define PB1 (*((volatile uint32_t *)0x40005008))
#define PB2 (*((volatile uint32_t *)0x40005010))
#define PB3 (*((volatile uint32_t *)0x40005020))
#define PB4 (*((volatile uint32_t *)0x40005040))
#define PC4 (*((volatile uint32_t *)0x40006040))
#define PC5 (*((volatile uint32_t *)0x40006080))
#define PC6 (*((volatile uint32_t *)0x40006100))
#define PC7 (*((volatile uint32_t *)0x40005200))



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

//PC4
volatile static unsigned long Touch2;     // true on touch
volatile static unsigned long Release2;   // true on release
volatile static unsigned long Last2;      // previous
void (*TouchTask2)(void);    // user function to be executed on touch
void (*ReleaseTask2)(void);  // user function to be executed on release

//PC5
volatile static unsigned long Touch3;     // true on touch
volatile static unsigned long Release3; // true on release
volatile static unsigned long Last3;      // previous
void (*TouchTask3)(void);    // user function to be executed on touch
void (*ReleaseTask3)(void);  // user function to be executed on release

//PC6
volatile static unsigned long Touch4;     // true on touch
volatile static unsigned long Release4;   // true on release
volatile static unsigned long Last4;      // previous
void (*TouchTask4)(void);    // user function to be executed on touch
void (*ReleaseTask4)(void);  // user function to be executed on release

//PC7
volatile static unsigned long Touch5;     // true on touch
volatile static unsigned long Release5;  // true on release
volatile static unsigned long Last5;    // previous
void (*TouchTask5)(void);    // user function to be executed on touch
void (*ReleaseTask5)(void);  // user function to be executed on release



static void Timer0Arm(void);
static void GPIOArm(void);


// Initialize switch interface on PF4 and pf0
// Inputs:  pointer to a function to call on touch (falling edge),
//          pointer to a function to call on release (rising edge)
// Outputs: none 
void Switch_Init(void(*touchtask)(void), void(*releasetask)(void),
										void(*touchtask1)(void),void(*releasetask1)(void));


// Interrupt on rising or falling edge of PF4 (CCP0)
void GPIOPortF_Handler(void);
 
// Interrupt 10 ms after rising edge of PF4
void Timer0A_Handler(void);

// Wait for switch to be pressed 
// There will be minimum time delay from touch to when this function returns
// Inputs:  none
// Outputs: none 
void Switch_WaitPress(void);

// Wait for switch to be released 
// There will be minimum time delay from release to when this function returns
// Inputs:  none
// Outputs: none 
void Switch_WaitRelease(void);

// Return current value of the switch 
// Repeated calls to this function may bounce during touch and release events
// If you need to wait for the switch, use WaitPress or WaitRelease
// Inputs:  none
// Outputs: false if switch currently pressed, true if released 
unsigned long Switch_Input(void);

 
static void GPIOArm1(void);

void Timer1A_Handler(void);

static void Timer1Arm(void);
//let's try to configure portc to work with the interrupts


void EdgeCounter_Init(void(*touchtask2)(void), void(*releasetask2)(void),
										void(*touchtask3)(void),void(*releasetask3)(void),
											void(*touchtask4)(void), void(*releasetask4)(void),
										void(*touchtask5)(void),void(*releasetask5)(void));

void GPIOPortC_Handler(void);
											
//End of file
											