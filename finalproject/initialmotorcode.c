/*
 * finalprojectmotor codeB.c
 *
 * Created: 4/3/2025 11:09:46 AM
 * Author : Lauren Ippolito
 */ 

 #include <avr/io.h>
 #define FREQ_CLK 1000000
 void wait(volatile int multiple);
 void doorsequence(void);
 void step_CW( void );
 void step_CCW( void );
 int phase_step = 1;

 int main(void)
 {
 //set up
 DDRB=1<<PB6|1<<PB3;//set motor controls as outputs
 
 while (1)//stay in loop
 {
 //wait specified time to open door, time will be determined by user inputs
 //for simplicity, we will wait 30 seconds to open door
 wait(1000);
 doorsequence();
 }
 }//end main


 void doorsequence()//open and closed door, 200 step motor
 {
 //cw 45 degrees in 1s
 for(int x=1;x<=100;x++) //need to move 45degrees/1.8=25 steps
 {
 step_CW();//one step CW
 wait(10);//(1sec/25)
 }
 
 //add function here to determine how long to wait based on weight sensor
 //for now just wait 4 seconds
 wait(4000);
 
 //ccw 45 degrees in 1s
 for(int x=1;x<=100;x++)//need to move 45degrees/1.8=25 steps
 {
 step_CCW();//one step CW
 wait(10);//(1sec/25)
 }
 
 }//end sequence

 //ccw
 void step_CCW()
 {
	 PORTB=0<<PB6|1<<PB3;//set DIR pin low for ccw
	 PORTB=0<<PB6|0<<PB3;
 }// end step_CCW

 //CW
 void step_CW() {
	PORTB=1<<PB6|1<<PB3;//set DIR pin high for cw
	PORTB=1<<PB6|0<<PB3;
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

