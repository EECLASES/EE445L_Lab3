
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