/*
 * finalprojectweightsensorcode.c
 *
 * Created: 4/16/2025 7:59:02 PM
 * Author : Lauren Ippolito
 */ 

#include <avr/io.h>
void dispensefood (int);
void step_CW( void );
void step_CCW( void );
#define FREQ_CLK 1000000
void wait(volatile int multiple);



int main(void)
{
	int foodchoice=2;
	int foodamount;
	if (foodchoice==1)
	{
		foodamount=130;
	}
	else if (foodchoice==2)
	{
		foodamount=180;//about 214g;
	}
	else if (foodchoice==3)
	{
		foodamount=220;
	}
	dispensefood(foodamount);
}

void dispensefood (int foodamount)
{
	
	//motor setup
		DDRB=1<<PB6|1<<PB3;//set motor controls (B3 and B6) as outputs
	//weight sensor setup
		DDRC=0<<PC1;//set weight sensor as input
		//AD conversion set up for sensor
		ADMUX=0b01100001;//reference AVcc, left justified, ADC1
		PRR=0x00;//clear power reduction bit
		ADCSRA = 0b10000111; // Set ADC Enable bit (7) in ADCSRA register, and set ADC prescaler to 128 
	// open food door
		 for(int x=1;x<=100;x++) //number of steps to open, about 45 degrees
		 {
			 step_CW();//one step CW
			  wait(10);
		 }
	
	//wait until weight sensor reaches predetermined weight (variable=food amount)
		char weightvalue=0;
		while(1)
		{
			// Read analog input
			ADCSRA = ADCSRA | 0b01000000; // Start conversion
			while ((ADCSRA & 0b00010000) == 0); // wait for conversion to finish
			weightvalue = ADCH; // Keep high byte of 10-bit result (throw away lowest two bits)
			if(weightvalue>=foodamount)
			{
				break;
			}else
			{
				//nothing keep checking
			}
		}
	
	//close food door
		 for(int x=1;x<=100;x++)//number of steps to close, about 45 degrees
		 {
			 step_CCW();//one step CW
			 wait(10);
		 }
}//end dispensefood function

//motor functions
void step_CW() {
	PORTB=1<<PB6|1<<PB3;//set DIR pin high for cw
	PORTB=1<<PB6|0<<PB3;
}// end step_CW
void step_CCW()
{
	PORTB=0<<PB6|1<<PB3;//set DIR pin low for ccw
	PORTB=0<<PB6|0<<PB3;
}// end step_CCW


//wait function
void wait(volatile int number_of_msec) {
	// This subroutine creates a delay equal to number_of_msec*T, where T is 1 msec
	// It changes depending on the frequency defined by FREQ_CLK
	char register_B_setting;
	char count_limit;
	
	// Some typical clock frequencies:
	switch(FREQ_CLK) {
		case 16000000:
		register_B_setting = 0b00000011; // this will start the timer in Normal mode with prescaler of 64 (CS02 = 0, CS01 = CS00 = 1).
		count_limit = 250; // For prescaler of 64, a count of 250 will require 1 msec
		break;
		case 8000000:
		register_B_setting =  0b00000011; // this will start the timer in Normal mode with prescaler of 64 (CS02 = 0, CS01 = CS00 = 1).
		count_limit = 125; // for prescaler of 64, a count of 125 will require 1 msec
		break;
		case 1000000:
		register_B_setting = 0b00000010; // this will start the timer in Normal mode with prescaler of 8 (CS02 = 0, CS01 = 1, CS00 = 0).
		count_limit = 125; // for prescaler of 8, a count of 125 will require 1 msec
		break;
	}

	while (number_of_msec > 0) {
		TCCR0A = 0x00; // clears WGM00 and WGM01 (bits 0 and 1) to ensure Timer/Counter is in normal mode.
		TCNT0 = 0;  // preload value for testing on count = 250
		TCCR0B =  register_B_setting;  // Start TIMER0 with the settings defined above
		while (TCNT0 < count_limit); // exits when count = the required limit for a 1 msec delay
		TCCR0B = 0x00; // Stop TIMER0
		number_of_msec--;
	}
} // end wait()
