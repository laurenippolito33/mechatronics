/*
 * lab7take3.c
 *
 * Created: 3/13/2025 10:16:30 PM
 * Author : Lauren Ippolito
 */ 

#include <avr/io.h>
#define FREQ_CLK 1000000
void wait(volatile int multiple);
void sequence(void);
void step_CW( void );
void step_CCW( void );
int phase_step = 1;
int numberofsteps=0;

int main(void)
{
    //set up
	DDRB=0b11111111;//set B to outputs for LEDs
	PORTB=0b11111111;//set LEDs off to start
	DDRC=0b00000000;//set C to inputs for switch
	DDRD=0b11111111;//set D to outputs for motor control
	
	while (1)//stay in loop
	{
		while (PINC & 0b00000001)
		{
			//do nothing, wait for button press
		}
		sequence();
	}
}//end main


void sequence()
{
	//cw 270 degrees in 2s
	PORTB=0b11111110;
	for(int x=1;x<=37.5;x++) //need to move 270degrees/1.8=150 steps/4=37.5
	{
		step_CW();//one step CW
		wait(53.33);//(2sec/37.5)
	}
	
	//wait
	PORTB=0b11111111;
	wait(500);
	
	//ccw 90 degrees in 1s
	PORTB=0b11111101;
	for(int x=1;x<=12.5;x++)//need to move 90degrees/1.8=50 steps/4=12.5
	{
		step_CCW();//one step CW
		wait(80);//(1sec/12.5)
	}
	
	//wait
	PORTB=0b11111111;
	wait(500);
	
	//cw 180 degrees in 2s
	PORTB=0b11111110;
	for(int x=1;x<=25;x++) //need to move 180degrees/1.8=100 steps/4=25
	{
		step_CW();//one step CW
		wait(80);//(2sec/25)
	}
	
	//stop and wait for button press
	PORTB=0b11111111;
	
}//end sequence

//ccw 
void step_CCW()
{
	// This function advances the motor counter-clockwise one step.  Follow the full-wave stepping table in Stepper Motor Control.ppt for MEMS 1049 course. phase1a = PORTD_7, phase1b = PORTD_6
	// phase2a = PORTD_5, phase2b = PORTD_4
	switch (phase_step) 
	{
		case 1:
		// step to 2
		PORTD = 0b00010000;
		phase_step = 2;
		break;
		case 2:
		// step to 3
		PORTD = 0b01000000;
		phase_step = 3;
		break;
		case 3:
		// step to 4;
		PORTD = 0b00100000;
		phase_step = 4;
		break;
		case 4:
		// step to 1;
		PORTD = 0b10000000;
		phase_step = 1;
		break;  
	}
}// end step_CCW

//CW

void step_CW() {
	// This function advances the motor clockwise one step.  Follow the full-wave stepping table in Stepper Motor Control.ppt for MEMS 1049 course. Our stepper motor phases are assigned to Port pins as follows: phase1a = PORTD_7, phase1b = PORTD_6, phase2a = PORTD_5, phase2b = PORTD_4
	switch (phase_step) 
	{
		case 1:
		// step to 4
		PORTD = 0b00100000;
		phase_step = 4;
		break;
		case 2:
		// step to 1
		PORTD = 0b10000000;
		phase_step = 1;
		break;
		case 3:
		// step to 2;
		PORTD = 0b00010000;
		phase_step = 2;
		break;
		case 4:
		// step to 3;
		PORTD = 0b01000000;
		phase_step = 3;
		break;  
	}
}// end step_CW


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


/* move to sequence to replace code to open box
//code to open box
for(int x=1;x<=115;x++)//need to move 828degrees/1.8=460 steps/4=115
{
	step_CCW();//one step CW
	wait(10);//(1sec/12.5)
}
wait(1000);
for(int x=1;x<=67.5;x++)//need to move 486degrees/1.8=270 steps/4=67.5
{
	step_CW();
	wait(10);
}
wait(1000);
for(int x=1;x<=17.5;x++)//need to move 126degrees/1.8= 70steps/4=17.5
{
	step_CCW();
	wait(10);
}
wait(1000);
*/
