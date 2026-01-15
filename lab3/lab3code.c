
 * Created: 2/7/2025 12:22:06 AM
 * Author : Lauren Ippolito
 */ 

#include <avr/io.h>
#define FREQ_CLK 1000000

//functions
void wait(volatile int);
void change_led_state(char);

int main(void)
{
	int number_of_states=22; //number of states in pattern
	char pattern[] = {'A','C','B','C','A','C','A','D','A','D','B','C','B','C','A','D','B','C','B','C','B','E'};
	//pattern of flashing, state E is the 2-second pause
	
	DDRD = 0b00000000; //set pins to inputs
	DDRC = 0b00000011; //PC0 and PC1 set to output for LEDs
	PORTC = 0b00000011; //turn off LEDs
	
	//Wait for switch to be pressed
	while (PIND & 0b00000100) //stay in loop if PD2 is high (switch not pressed)
	{//does nothing, waiting for button to be pressed
	}
	while (1) //repeat pattern 
	{
		for (int i=0; i<number_of_states;i++) //move through LED states
		{
			change_led_state(pattern[i]);//pass current pattern state to function to change to new state
		}
	}
	return(0);
}

void change_led_state(char new_state)
{
	//function to move between states
	switch (new_state)
	{
		case 'A':
			PORTC=0b00000000; //turn on 
			wait (200); //short period
		break;
		case 'B':
			PORTC=0b00000000; //turn on
			wait(600);//long period
		break;
		case 'C':
			PORTC=0b00000011; //turn off
			wait(200); //short period
		break;
		case 'D':
			PORTC=0b00000011; //turn off
			wait(600); //long period
		break;
		default: //off period 
			PORTC=0b00000011;
			wait(2000); //two seconds
	}
	while (!(PIND & 0b00000100)) //stay in loop if switch is pressed
	{
		wait(50);//for debounce
	}
} 

void wait(volatile int number_of_msec)
{
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
	
}


