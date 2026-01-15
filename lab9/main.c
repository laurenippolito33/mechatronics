/*
 * lab9practicetry2.c
 *
 * Created: 3/28/2025 1:40:13 AM
 * Author : Lauren Ippolito
 */ 


#include <avr/io.h>
#define FREQ_CLK 1000000
void wait(volatile int);
int sendtomax(unsigned char command, unsigned char data);

int main(void)
{
	
	DDRD = 0xFF;   // Set DDRD to display data that comes in from Secondary
	DDRB = 0b00101100; //	DDRB = 1<<PORTB5 | 1<<PORTB3 | 1<<PORTB2;  // Set pins SCK, MOSI, and SS as output
	//initialize SPI
	SPCR = 0b01010001; // SPCR = 1<<SPE | 1<<MSTR | 1<<SPR0; // (SPIE = 0, SPE = 1, DORD = 0, MSTR = 1, CPOL = 0, CPHA = 0, SPR1 = 0, SPR0 = 1)
					// enable the SPI, set to Main mode 0, SCK = Fosc/16, lead with MSB
	//enable decode mode
	char decode1 = 0b00001001;
	char decode2 = 0b00000011;
	sendtomax(decode1,decode2);
	//set scan limit
	char scan1 = 0b00001011;
	char scan2 = 0b00000010;
	sendtomax(scan1, scan2);
	//turn on delay
	char turnon1 = 0b00001100;
	char turnon2 = 0b00000001;
	sendtomax(turnon1, turnon2);
	
	
	char screen1 = 0b00000001; // screen 1
	char screen2 = 0b00000010; // screen2
	
	while (1)
	{
		
		// display month
		char month0 = 0b00000000; // this is the data to display a zero
		char month3 = 0b00000011; // this will display a 3
		sendtomax(screen1, month0);
		sendtomax(screen2, month3);
		// wait 1 sec
		wait(1000);
		// display day
		char date2 = 0b00000010; // displays a 2
		char date8 = 0b00001000; // displays an 8
		sendtomax(screen1, date2);
		sendtomax(screen2, date8);
		wait(1000);
		// display blank
		char blankdata= 0b00001111; // displays blank
		sendtomax(screen1,blankdata);
		sendtomax(screen2,blankdata);
		wait(2000);
	}
	
}


void wait(volatile int number_of_msec)
{
	char register_B_setting;
	char count_limit;
	
	// allows for different clock frequencies and sets the settings and number of counts it takes to his 1 ms
	switch(FREQ_CLK) {
		case 16000000:
		register_B_setting = 0b00000011;
		count_limit = 250;
		break;
		case 8000000:
		register_B_setting = 0b00000011;
		count_limit = 125;
		break;
		case 1000000:
		register_B_setting = 0b00000010;
		count_limit = 125;
		break;
	}
	// TIME KILLING LOOP
	while(number_of_msec > 0)
	{
		TCCR0A = 0x00;
		TCNT0 = 0; // clears timer register at start
		TCCR0B = register_B_setting; // starts timer with the correct settings from above
		while (TCNT0 < count_limit); // timer counts until it hits enough counts to be 1 ms
		TCCR0B = 0x00; // stops timer
		number_of_msec--; // keeps count of number of ms that are left to go
	}
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

