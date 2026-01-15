/*
 * GccApplication1.c
 *
 * Created: 3/28/2025 12:04:56 AM
 * Author : Lauren Ippolito
 */ 

#include <avr/io.h>
#define FREQ_CLK 1000000
void wait(volatile int multiple);
int sendtomax(unsigned char command, unsigned char data);


int main(void)
{
   //set up 
  DDRD = 0xFF;   // Set DDRD to display data that comes in from Secondary
  DDRB = 0b00101100; //	DDRB = 1<<PORTB5 | 1<<PORTB3 | 1<<PORTB2;  // Set pins SCK, MOSI, and SS as output
   //initialize SPI to operate in main mode 0
  SPCR = 0b01010001; // SPCR = 1<<SPE | 1<<MSTR | 1<<SPR0; // (SPIE = 0, SPE = 1, DORD = 0, MSTR = 1, CPOL = 0, CPHA = 0, SPR1 = 0, SPR0 = 1)
					 // enable the SPI, set to Main mode 0, SCK = Fosc/16, lead with MSB
   //enable/disable decoding mode for desired digits
   char decode1 = 0b00001001;
   char decode2 = 0b00000011l;
   sendtomax(decode1,decode2);
   //set scan limit (number of digits on display)
   char scan1 = 0b00001011;
   char scan2 = 0b00000010;
   sendtomax(scan1,scan2);
   //turn on display
   char turnon1 = 0b00001100;
   char turnon2 = 0b00000001;
   sendtomax(turnon1,turnon2);
   
   char  screen1=0b00000001;//use to set screen 1
   char  screen2=0b00000010;//use to set screen 2
   while(1) //send values for digits
   {
	   
	   //display month 03 for 1 second
	   char month0= 0b00000000;//0
	   char month3=0b00000011;//3
	   sendtomax(screen1,month0); //set screen 1 to 0
	   sendtomax(screen2,month3); //set screen 2 to 3
	   wait(1000); //wait 1s
	   //display the day 29 for 1 second
	   char date2= 0b00000010;//2
	   char date9= 0b00001111;//9
	   sendtomax(screen1,date2);//set screen 1 to 2
	   sendtomax(screen2,date9);//set screen 2 to 9
	   wait(1000);//wait 1s
	   //blank display for 2 seconds
	   char blank=0b00001111;//blank
	   sendtomax(screen1,blank);
	   sendtomax(screen2,blank);
	   wait(2000);
	   
   }//repeat indefinably
   
}

int sendtomax(unsigned char command, unsigned char data)
{
	PORTB = PORTB & 0b11111011; // Clear PB2, which is the SS bit, so that
	// transmission can begin
	SPDR = command; // Send command
	while(!(SPSR & (1<<SPIF))); // Wait for transmission to finish
	SPDR = data; // Send data
	while(!(SPSR & (1<<SPIF))); // Wait for transmission to finish
	PORTB = PORTB | 0b00000100; // Return PB2 to 1, which is the SS bit, to end
	// transmission
	return 0;
}



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
