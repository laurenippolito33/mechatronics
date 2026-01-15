/*
 * finalprojectbreakbeamcodeA.c
 *
 * Created: 4/9/2025 11:26:23 AM
 * Author : Lauren Ippolito
 */ 

#include <avr/io.h>
char LEDvalue=0;

int main(void)
{
	//breakbeam set up
	DDRC = 0<<PC3; // Make PB4  input bit for break beam
	//AD conversion set up
	ADMUX=0b01100011;//reference AVcc, left justified, ADC3
	PRR=0x00;//clear power reduction bit
	ADCSRA = 0b10000111; // Set ADC Enable bit (7) in ADCSRA register, and set ADC prescaler to 128 (bits 2-0 of ADCSRA = ADPS2-ADPS0 = 111)
	
	//read analog input from breakbeam 
	ADCSRA = ADCSRA | 0b01000000; //Alternate code: ADCSRA |= (1<<ADSC); // Start conversion
	while ((ADCSRA & 0b00010000) == 0); //  wait for conversion to finish
	LEDvalue = ADCH; // Keep high byte of 10-bit result (throw away lowest two bits) (0-255 range)
		if(LEDvalue>=60)//Beam is not break, need to add food to container
		{
	
		}
		else if (LEDvalue<=60)//beam is broken
		{//do nothing
		}
	}
	
}
