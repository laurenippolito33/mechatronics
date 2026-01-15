/*
 * lab10code1.c
 *
 * Created: 4/2/2025 2:21:32 PM
 * Author : Lauren Ippolito
 */ 


#include <stdio.h>
#include <math.h>
#include <avr/io.h>
#define FREQ_CLK 1000000
void wait(volatile int);
int sendtomax(unsigned char command, unsigned char data);

void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_repeated_start(void);
void i2c_write_to_address(unsigned char address);
void i2c_read_from_address(unsigned char address);
void i2c_write_data(unsigned char data);
unsigned char i2c_read_data(unsigned char ack);



int main(void)
{
	//set up
	DDRD = 0xFF;   // Set DDRD to display data that comes in from Secondary
	DDRB = 0b00101100; //	DDRB = 1<<PORTB5 | 1<<PORTB3 | 1<<PORTB2;  // Set pins SCK, MOSI, and SS as output
	DDRC = 0b00000000; // Define all PORTC bits as input (specifically PC4 and PC5)
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
	//which screen to use
	char screen1 = 0b00000001; // screen 1
	char screen2 = 0b00000010; // screen2
	
	// I2C (TWI) Setup
	PRR = PRR & 0b01111111; // Ensure that TWI is powered on (PRTWI = 0)
	DDRC = 0b00000000; // Define all PORTC bits as input (specifically PC4 and PC5)
	PORTC = PORTC | 0b00110000;  // set internal pull-up resistors on SCL and SDA lines (PC5 and PC4) ofr I2C bus
	i2c_init();
	
	unsigned char data_low;
	unsigned char data_high;
	char Temperature;
	char TemperatureF;
	
	// Set Resolution
	char res_data = 0x00;
	i2c_start();
	i2c_write_to_address(0x18);
	i2c_write_data(0x08);
	i2c_write_data(res_data);
	i2c_stop();
	
	while (1)
	{
		//read in temp from sensor
		i2c_start();
		i2c_write_to_address(0x18);
		i2c_write_data(0x05);
		i2c_repeated_start();
		i2c_read_from_address(0x18); // reads ambient temp
		data_high = i2c_read_data(1); // read with ACK
		data_low = i2c_read_data(0);  // read with NO_ACK
		i2c_stop();
		// convert data to a number
		data_high = data_high & 0x1F; //Clear upper three flag bits
		if ((data_high & 0x10) == 0x10){ //TA < 0°C
			data_high = data_high & 0x0F; //Clear SIGN bit
			Temperature = 256 - (data_high * 16 + data_low / 16);
		}else //TA > 0°C
		Temperature = (data_high * 16 + data_low / 16);
		
		TemperatureF = (Temperature*(9.0/5.0))+32;
		
		char digit1 = floor(TemperatureF/10);
		char digit2 = TemperatureF - digit1*10;
	
		
		// display temp
		sendtomax(screen1, digit1);
		sendtomax(screen2, digit2);
		wait(1000);
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


void i2c_init(void) { // initialize i2c
	TWSR = 0b00000000; // prescaler is set to 1
	TWBR = 72; // Put 72 into TWBR to define SCL frequency as 100kHz for 16MHz oscillator
	TWCR = 0b00000100; // TWEN = 1 (enable TWI)
}

void i2c_start(void) { // send start command
	//while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000));   // wait for idle condition -- TWINT bit must be high to proceed -- not needed if single main is used
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);  //TWCR | 0b10100100;       // initiate START condition -- write 1 to TWINT to clear it and initiate action, set TWSTA bit, set TWEN bit
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000));   // wait for action to finish (poll TWINT bit)
	// if ((TWSR & 0xF8) != START) // error checking -- need to predefine START = 0x08 and ERROR() function.
	// ERROR();
}
	
void i2c_read_from_address(unsigned char address) { //Write an address byte to the I2C bus in form of SLA_R (address to read from)
	unsigned char SLA_R = address<<1 | 1; // create SLA_R byte by shifting address and setting R/W bit
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000)) ;  // wait for idle condition -- TWINT bit must be high to proceed
	TWDR = SLA_R;       // Load TWDR with address plus R/W bit
	TWCR = (1<<TWINT) | (1<<TWEN); //TWCR = TWCR | 0b10000100;       // initiate Write -- write 1 to TWINT to clear it and initiate action, and set TWEN
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000)) ;  // wait for action to finish (poll TWINT bit)
	// if ((TWSR & 0xF8) != MR_SLA_ACK) // error checking -- need to predefine MR_SLA_ACK and ERROR() function depending on possible outcomes: 0x38, 0x40, or 0x48.
	//ERROR();
}

unsigned char i2c_read_data(unsigned char ACK) { //Read a byte of data from a secondary on the I2C2 bus
	unsigned char data;
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000)) ;  // wait for idle condition -- TWINT bit must be high to proceed
	if (ACK) // check for whether ACK or NO_ACK should be sent upon receipt of byte from secondary
	TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN); //TWCR = TWCR | 0b11000100;       // initiate Read with ACK -- write 1 to TWINT to clear it and initiate action, and set TWEA and TWEN
	else
	TWCR = (1<<TWINT) | (1<<TWEN); //TWCR = TWCR | 0b10000100;       // initiate Read with NO_ACK-- write 1 to TWINT to clear it and initiate action, and set TWEN
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000)) ;  // wait for action to finish (poll TWINT bit)
	// if ((TWSR & 0xF8) != MR_SLA_ACK) // error checking -- need to predefine MR_SLA_ACK and ERROR() function depending on possible outcomes: 0x50 or 0x58.
	//ERROR();
	//If multiple bytes are to be read, this function can be repeated with proper ACK or NO_ACK until done.
	data = TWDR;  // read the received data from secondary
	return(data);
}

void i2c_stop(void) { // send stop command
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000)) ;  // wait for action to finish (poll TWINT bit)
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO); //TWCR = TWCR | 0b10010100;       // initiate STOP condition -- write 1 to TWINT to clear it and initiate action, set TWSTO bit and set TWEN bit
}


void i2c_repeated_start(void) {
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);  //TWCR | 0b10100100;       // initiate START condition -- write 1 to TWINT to clear it and initiate action, set TWSTA bit, set TWEN bit
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000));   // wait for action to finish (poll TWINT bit)
	// if ((TWSR & 0xF8) != START) // error checking -- need to predefine START = 0x10 and ERROR() function.
	// ERROR();
}

void i2c_write_to_address( unsigned char address) { //Write an address byte to the I2C2 bus in form of SLA_W (address to write to)
	unsigned char SLA_W = address<<1; // create SLA_W byte by shifting address and leaving R/W bit clear
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000)) ;  // wait for idle condition -- TWINT bit must be high to proceed
	TWDR = SLA_W;       // Load TWDR with address plus R/W bit
	TWCR = (1<<TWINT) | (1<<TWEN); //TWCR = TWCR | 0b10000100;       // initiate Write -- write 1 to TWINT to clear it and initiate action, and set TWEN
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000)) ;  // wait for action to finish (poll TWINT bit)
	// if ((TWSR & 0xF8) != MT_SLA_ACK) // error checking -- need to predefine MT_SLA_ACK and ERROR() function depending on possible outcomes: 0x18, 0x20, or 0x38.
	//ERROR();
}


void i2c_write_data( unsigned char data) { //Write data byte to the I2C2 bus
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000)) ;  // wait for idle condition -- TWINT bit must be high to proceed
	TWDR = data;       // Load TWDR with data to be sent
	TWCR = (1<<TWINT) | (1<<TWEN); //TWCR = TWCR | 0b10000100;       // initiate Write -- write 1 to TWINT to clear it and initiate action, and set TWEN
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000)) ;  // wait for action to finish (poll TWINT bit)
	// if ((TWSR & 0xF8) != MT_DATA_ACK) // error checking -- need to predefine MT_DATA_ACK and ERROR() function depending on possible outcomes: 0x28 or 0x30.
	//ERROR();
}



