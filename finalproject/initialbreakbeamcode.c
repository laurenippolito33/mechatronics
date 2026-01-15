/*
 * finalprojectbreakbeamcodeA.c
 *
 * Created: 4/9/2025 11:26:23 AM
 * Author : Lauren Ippolito
 */ 

#include <avr/io.h>
char sensorvalue=0;

int main(void)
{
	
	DDRD = 1<<PD4;// Make PD4 output bit (for LED)
	PORTD=1<<PD4;
	DDRC = 0<<PC3; // Make PB4  input bit for break beam
	//AD conversion set up
	ADMUX=0b01100011;//reference AVcc, left justified, ADC3
	PRR=0x00;//clear power reduction bit
	ADCSRA = 0b10000111; // Set ADC Enable bit (7) in ADCSRA register, and set ADC prescaler to 128 (bits 2-0 of ADCSRA = ADPS2-ADPS0 = 111)
	
	while(1)
	{
		//read analog input from breakbeam 
		ADCSRA = ADCSRA | 0b01000000; //Alternate code: ADCSRA |= (1<<ADSC); // Start conversion
		while ((ADCSRA & 0b00010000) == 0); //  wait for conversion to finish
		sensorvalue = ADCH; // Keep high byte of 10-bit result (throw away lowest two bits) (0-255 range)
		if(sensorvalue<=60)//Beam is mostly broken
		{
			PORTD=1<<PD4;//turn LED off
		}
		else if (sensorvalue>=60)//beam is not broken,, feed dog
		{
			PORTD=0<<PD4;//turn led on
		}
	}
	
}

