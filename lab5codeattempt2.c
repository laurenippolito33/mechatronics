/*
 * GccApplication1.c
 *
 * Created: 2/20/2025 11:07:20 PM
 * Author : Lauren Ippolito
 */ 

#include <avr/io.h>
//global variables
char results=0;//value read from analog sensor(0-255)
int voltage=0;
int main(void)
{
	//setup
	DDRC=0b00011111; //set PC 0-4 as outputs for LEDs 0-4 and PC5 as input for potentiometer
	PORTC=0b00011111;//turn off LEDs to start
	PRR = 0x00;  // clear Power Reduction ADC bit (0) in PRR register
	ADCSRA = 0b10000111; //1<<ADEN | 1<<ADPS2 | 1<<ADPS1 | 1<<ADPS0;  // 0x87 // 0b10000111 // Set ADC Enable bit (7) in ADCSRA register, and set ADC prescaler to 128 (bits 2-0 of ADCSRA = ADPS2-ADPS0 = 111)
	ADMUX = 0b00100101; //AVcc power, left justified and channel 5

	while(1)
	{
	
		// Read analog input
		ADCSRA = ADCSRA | 0b01000000; //Alternate code: ADCSRA |= (1<<ADSC); // Start conversion
		while ((ADCSRA & 0b00010000) == 0); // Alternate code: while ((ADCSRA & (1<<ADIF)) ==0); // wait for conversion to finish
		{
			results = ADCH;//0-255 range
			if(results>=204)
			{
				voltage=5;
			}else if(results>=153)
			{
				voltage=4;
			}else if(results>=102)
			{
				voltage=3;
			}else if(results>=51)
			{
				voltage=2;
			}else
			{
				voltage=1
			}
			
			//change LED based on voltage
			switch(voltage)
			{
				case 1:
				PORTC=0b11111110;
				break;
				case 2:
				PORTC=0b11111101;
				break;
				case 3:
				PORTC=0b11111011;
				break;
				case 4:
				PORTC=0b11110111;
				break;
				case 5:
				PORTC=0b11101111;
				break;
			}
		}
			
	}
}

