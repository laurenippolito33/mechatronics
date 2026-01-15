/*
 * lab6mechatronics1.c
 *
 * Created: 2/27/2025 8:49:48 PM
 * Author : Lauren Ippolito
 */ 

#include <avr/io.h>
//global variables
char results;//value read from analog sensor(0-255)
int voltage=0;
int main(void)
{
	//setup
	DDRD=0b11111111;//set port D as outputs, PD0 and PD1 are motor controls
	DDRC=0b00011111; //set PC 0-4 as outputs for LEDs 0-4 and PC5 as input for potentiometer
	PORTC=0b11111111;//turn off LEDs to start
	PRR = 0x00;  // clear Power Reduction ADC bit (0) in PRR register
	ADCSRA = 0b10000111; //1<<ADEN | 1<<ADPS2 | 1<<ADPS1 | 1<<ADPS0;  // 0x87 // 0b10000111 // Set ADC Enable bit (7) in ADCSRA register, and set ADC prescaler to 128 (bits 2-0 of ADCSRA = ADPS2-ADPS0 = 111)
	ADMUX = 0b00100101; //AVcc power, left justified and channel 5
	//PWM setup
	OCR0A=0x00;
	TCCR0A=0b10000011; //set pin PD6 to with bits 6,7 to 10 to set non inverting mode (timer counter for motor) and bits 0,1 to 11 for fast PWN
	TCCR0B=0b00000011; // bits 0,1,2 to set prescaler 011 for prescaler 64

	while(1)
	{
		
		// Read analog input
		ADCSRA = ADCSRA | 0b01000000; //Alternate code: ADCSRA |= (1<<ADSC); // Start conversion
		while ((ADCSRA & 0b00010000) == 0); // Alternate code: while ((ADCSRA & (1<<ADIF)) ==0); // wait for conversion to finish
		{
			results = ADCH;//0-255 range
			
			//motor direction and speed
			if(results>127.5)//voltage from 2.5 to 5 v
			{
				PORTD=0b00000001;//motor moves forward
				OCR0A=results-127.5;//motor increasing speed  from 0% at 2.5V to 50% at 
			}else if(results<127.5)//voltage 0 to 2.5 v
			{
				PORTD=0b00000010;//motor moves backward
				OCR0A=127.5-results; //motor decreasing speed from 50% at 0V to 0% at 2.5 V to 
			}else //voltage=2.5
			{
				PORTD=0b00000000;//motor stops
			}
			
			//LED states
			if(results>=204)
			{
				PORTC=0b11101111;//turn on LED 4
				//voltage=4-5
			}else if(results>=153)
			{
				PORTC=0b11110111;//turn on LED 3
				//voltage=3-4
			}else if(results>=102)
			{
				PORTC=0b11111011;
				//voltage=2-3
			}else if(results>=51)
			{
				PORTC=0b11111101;//turn on LED 1
				//voltage=1-2
			}else
			{
				PORTC=0b11111110;//turn on LED 0
				//voltage=0-1
			}
			
		}//end while loop

	}//end main while
}//end main



