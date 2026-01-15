//final project code with all components combined. Written by Lauren Ippolito, Olivia Hendrickson, and Anna George

  /*
 * MEMS1049_FinalProject.c
 *
 * Created: 4/19/2025 9:32:50 PM
 * Author : orhen
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#define FREQ_CLK 1000000

#define lcd_port PORTD   // LCD uses PORTD
#define lcd_EN   1       // Enable pin
#define lcd_RS   0       // Register select pin

// Global variables
char sensorvalue = 0;
char beamvalue   = 0;

int gouser     = 0;
int foodamount = 0;
int flag       = 0;

// Timer / interrupt variables
volatile int  loop = 0;
volatile int  number_of_msec = 30000;
volatile char register_B_setting;
volatile char timer_preload;

// Function prototypes
void wait(volatile unsigned int);
void LCD_write(unsigned char cmd);
void LCD_init(void);
void LCD_command(char);
void LCD_data(char);
void LCD_print(char *str);
void LCD_gotoxy(unsigned char x, unsigned char y);

// Motor functions
void dispensefood(int);
void step_CW(void);
void step_CCW(void);

/* ------------------------------------------------------------
 * TIMER2 Overflow Interrupt
 * ------------------------------------------------------------ */
ISR(TIMER2_OVF_vect)
{
    loop++;

    if (loop == number_of_msec)
    {
        TIFR2 = 1 << TOV2;   // Clear overflow flag
        loop = 0;
        flag = 1;
    }
}

/* ------------------------------------------------------------
 * Main
 * ------------------------------------------------------------ */
int main(void)
{
    /* Pin setup */
    DDRC = 0 << PC3;        // Break beam input
    DDRC = 0 << PC1;        // Weight sensor input

    DDRB = 1 << PB3;        // Motor control
    DDRB = 1 << PB6;

    DDRB = 0 << PB0;        // Switches
    DDRB = 0 << PB1;
    DDRB = 0 << PB2;

    DDRD = 0xFF;            // LCD output pins
    DDRC = (1 << PC2) | (1 << PC4);  // Water valve control

    /* Timer2 setup */
    switch (FREQ_CLK)
    {
        case 16000000:
            register_B_setting = 0b00000011;
            timer_preload = 255 - 250;
            break;

        case 8000000:
            register_B_setting = 0b00000011;
            timer_preload = 255 - 125;
            break;

        case 1000000:
            register_B_setting = 0b00000010;
            timer_preload = 255 - 125;
            break;
    }

    TCCR2A = 0x00;
    TIMSK2 = 1 << TOIE2;
    TCNT2  = timer_preload;
    sei();
    TCCR2B = register_B_setting;

    /* LCD setup */
    LCD_init();
    wait(50);

    LCD_gotoxy(1, 1);
    LCD_print("1=0.25cup: 2= ");

    LCD_gotoxy(1, 2);
    LCD_print("0.5cup: 3=1cup ");

    wait(500);

    /* --------------------------------------------------------
     * Main loop
     * -------------------------------------------------------- */
    while (1)
    {
        /* User input (run once) */
        while (gouser == 0)
        {
            if (!(PINB & 0b00000001))
            {
                LCD_gotoxy(1, 1);
                LCD_print(" 1");
                wait(1000);

                foodamount = 90;
                gouser = 1;
            }
            else if (!(PINB & 0b00000010))
            {
                LCD_gotoxy(1, 1);
                LCD_print(" 2");
                wait(1000);

                foodamount = 130;
                gouser = 1;
            }
            else if (!(PINB & 0b00000100))
            {
                LCD_gotoxy(1, 1);
                LCD_print(" 3");
                wait(1000);

                foodamount = 180;
                gouser = 1;
            }
        }

        /* Feeding event */
        if (flag == 1)
        {
            dispensefood(foodamount);

            /* Break beam ADC */
            ADMUX  = 0b01100011;
            PRR    = 0x00;
            ADCSRA = 0b10000111;
            ADCSRA |= 0b01000000;

            while ((ADCSRA & 0b00010000) == 0);
            beamvalue = ADCH;

            if (beamvalue >= 60)
            {
                LCD_gotoxy(1, 1);
                LCD_print("Food Supply Low ");

                LCD_gotoxy(1, 2);
                LCD_print("Refill Container");

                wait(10000);
            }

            flag = 0;
        }

        /* Water valve control */
        PORTC |= 0b00000100;

        ADCSRA = 0b10000111;
        ADMUX  = 0b00100000;

        if (PINC & 0b00100000)
        {
            ADCSRA |= 0b01000000;
            while ((ADCSRA & 0b00010000) == 0);

            sensorvalue = ADCH;

            if (sensorvalue < 100)
                PORTD |= (1 << PD3);
            else
                PORTD &= ~(1 << PD3);
        }
        else
        {
            PORTD &= ~(1 << PD3);

            LCD_gotoxy(1, 1);
            LCD_print("Water Supply Low");

            LCD_gotoxy(1, 2);
            LCD_print("Refill Reservoir");

            wait(5000);
        }
    }
}

/* ------------------------------------------------------------
 * LCD Functions
 * ------------------------------------------------------------ */
void LCD_init(void)
{
    LCD_command(0x02);
    wait(1);
    LCD_command(0x28);
    wait(1);
    LCD_command(0x01);
    wait(1);
    LCD_command(0x0E);
    wait(1);
    LCD_command(0x80);
    wait(1);
}

void LCD_gotoxy(unsigned char x, unsigned char y)
{
    unsigned char firstCharAdr[] = {0x80, 0xC0, 0x94, 0xD4};
    LCD_command(firstCharAdr[y - 1] + x - 1);
    wait(1);
}

void LCD_command(char command_value)
{
    lcd_port = command_value & 0xF0;
    lcd_port &= ~(1 << lcd_RS);
    lcd_port |= (1 << lcd_EN);
    wait(1);
    lcd_port &= ~(1 << lcd_EN);

    lcd_port = (command_value << 4) & 0xF0;
    lcd_port &= ~(1 << lcd_RS);
    lcd_port |= (1 << lcd_EN);
    wait(1);
    lcd_port &= ~(1 << lcd_EN);
}

void LCD_data(char data_value)
{
    lcd_port = data_value & 0xF0;
    lcd_port |= (1 << lcd_RS);
    lcd_port |= (1 << lcd_EN);
    wait(1);
    lcd_port &= ~(1 << lcd_EN);

    lcd_port = (data_value << 4) & 0xF0;
    lcd_port |= (1 << lcd_RS);
    lcd_port |= (1 << lcd_EN);
    wait(1);
    lcd_port &= ~(1 << lcd_EN);
}

void LCD_print(char *str)
{
    int i = 0;
    while (str[i] != '\0')
    {
        LCD_data(str[i]);
        i++;
    }
}

/* ------------------------------------------------------------
 * Motor Functions
 * ------------------------------------------------------------ */
void dispensefood(int foodamount)
{
    ADMUX  = 0b01100001;
    PRR    = 0x00;
    ADCSRA = 0b10000111;

    for (int x = 0; x < 100; x++)
    {
        step_CW();
        wait(10);
    }

    char weightvalue = 0;
    while (1)
    {
        ADCSRA |= 0b01000000;
        while ((ADCSRA & 0b00010000) == 0);
        weightvalue = ADCH;

        if (weightvalue >= foodamount)
            break;
    }

    for (int x = 0; x < 100; x++)
    {
        step_CCW();
        wait(10);
    }
}

void step_CW(void)
{
    PORTB = 1 << PB6;
    PORTB = 1 << PB3;
    PORTB = 0 << PB3;
}

void step_CCW(void)
{
    PORTB = 0 << PB6;
    PORTB = 1 << PB3;
    PORTB = 0 << PB3;
}
