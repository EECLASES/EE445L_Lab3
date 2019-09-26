//This is the Function header for all SoundDriver prototypes and definitions

#define PD0 (*((volatile unsigned long *) 0x40007004))

	
void PC5toggle(void);

void Sound_Init(void);

void playAlarm(void);

