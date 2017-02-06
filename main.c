/*
 * AVR_C_player.c
 *
 * Created: 24/01/2017 10:11:33
 * Author : josefe
 */ 

#define F_CPU 20000000UL //20 Mhz clock

#include <avr/io.h> //Port handling
#include <avr/interrupt.h> //Interrupts handling
#include <avr/sleep.h> //Sleep management
#include <avr/pgmspace.h> //Flash rom management
#include <util/delay.h>
#include "rom.h" //Data

#define end_of_data		0 
#define port_disabled	128 //Value to be set if the output is off

#define startup_delay	300

// Timer setup
#define timer_count		250
#define timer_pres		2
#define timer_int_en	2

// Data buffers
volatile unsigned char data_buffer;
volatile unsigned int data_index; 

ISR(TIMER0_COMPA_vect)
{
	PORTD = data_buffer; //Update port
	data_index++; //Read and latch next data
	data_buffer = pgm_read_byte(&(rom[data_index]));
}


int main(void)
{
	//Sleep init
	set_sleep_mode(SLEEP_MODE_PWR_DOWN); //Max poer saving mode
	PRR = 0b11001111; //Disable all peripherals except timer 0
	
	//Port init
	DDRD = 0xFF; //Output port
	PORTD = port_disabled; //Half value
	
	//SW init
	data_index = 0;
	data_buffer = pgm_read_byte(&(rom[data_index])); //Load first data
	
	_delay_ms(startup_delay);	//Allows analog hardware to be ready before playing
	
	//Timer init: output compare with interrupts, reset on match, Fck/8 prescaler setup
	OCR0B = 0;
	TCNT0 = 0;
	TCCR0A = 0;
	OCR0A = timer_count;
	TIMSK0 = timer_int_en;
	sei();					//Enable interrupts
	TCCR0B = timer_pres;	//Turn timer on

    
    while (1) 
    {
		if (data_buffer==end_of_data) //if data is 0 (EOF)
		{
			PORTD=port_disabled; //turns off output
			cli(); //Disables interrupts
			sleep_enable(); // Hard sleep (only POR or EXTR can wake it up)
			sleep_mode();
		
		}
		
    }
}

