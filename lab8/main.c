/*
 * lab8code1.c
 *
 * Created: 3/18/2025 9:18:29 PM
 * Author : Lauren Ippolito
 */ 

#include <avr/io.h>
//chip 1
void initialize_usart(void); // function to set up USART
void transmit_data_usart(int); // function to transmit a byte by USART
#define FREQ_CLK 1000000 // Clock frequency
void wait(volatile int multiple);


int main(void)
{
	int data;
	
	//set PWM for LED brightness control to inverting mode
	//PWM setup
	DDRD=0b01111111;//set D as output for PWM at PD6 for led
	DDRD=0b11111111;//turn off to start
	OCR0A=0x00;
	TCCR0A=0b11000011; //set pin PD6 to with bits 6,7 to 11 to set inverting mode and bits 0,1 to 11 for fast PWN
	TCCR0B=0b00000011; // bits 0,1,2 to set prescaler 011 for prescaler 64
	
	initialize_usart(); // Initialize the USART with desired parameters
	//Wait 1 sec
	wait(1000);
	//transmit first value
	transmit_data_usart(0xFE);
	//receive data back from chip 2
	while (!(UCSR0A & 0b10000000)){}; // ((! (UCSR0A & (1<<RXC0)));  // Wait until new data arrives
	data =  UDR0; // read the data
	OCR0A=data;
	
	wait(3000);
	//transmit second value
	transmit_data_usart(0xFF);
	//receive data back from chip 2
	while (!(UCSR0A & 0b10000000)){}; // ((! (UCSR0A & (1<<RXC0)));  // Wait until new data arrives
	data =  UDR0; // read the data
	OCR0A=data;
	
	wait(3000);
	//transmit third value
	transmit_data_usart(0x55);
	//receive data back from chip 2
	while (!(UCSR0A & 0b10000000)){}; // ((! (UCSR0A & (1<<RXC0)));  // Wait until new data arrives
	data =  UDR0; // read the data
	OCR0A=data;

}//end main


void initialize_usart(void) // function to set up USART
{
	UCSR0B = (1<<TXEN0); // enable serial transmission
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00) ; // Asynchronous mode, 8-bit data; no parity; 1 stop bit
	UBRR0L = 0x67; // 9,600 baud if Fosc = 16MHz
}

void transmit_data_usart(int data) // Function to transmit a character
{
	while (!(UCSR0A & (1<<UDRE0))); // Poll to make sure transmit buffer is ready, 	// then send data
	UDR0 = data;
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