/*
 * bandvagn.c
 *
 * Created: 2020-12-04 10:01:40
 * Author : 46766
 */ 
#include "globaldefs.h"
#include "ADC_util.h"
#include "UART_util.h"
#include "Digit_handler.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>
#include <util/delay.h>


volatile unsigned char received_byte;

unsigned char message[] = "button pressed! \n";

volatile unsigned int range = 0;
volatile unsigned int range_tot = 0;
volatile unsigned int range_arr[70];


unsigned char array[20];
volatile unsigned char i = 0;


int main(void)
{
	PORTC = 0xff;	//pull-up
	DDRC = 0;		//in	DDC0 = button
	
	PORTD = 0xff;
	DDRD = (1<<DDD6)|(1<<DDD5)|(1<<DDD4)|(1<<DDD3)|(1<<DDD2)|(1<<DDD1);	//Out (leds, TX)
	
	init_ADC();
	usart_init();
	_delay_ms(200);
	
	ADCSRA	|= (1<<ADSC);	//start reading

    while (1) 
    {
		
		if (bit_is_clear(PINC, DDC0)) //testknapp
		{
			send_in_ASCII(9999);
			usart_send_string(message);
			FLIPBIT(PORTD, DDD4);
		
			_delay_ms(400);
		}
		
		//skicka varje sekund
		send_in_ASCII((range_tot/70));
		
		
		FLIPBIT(PORTD, DDD5);
		_delay_ms(500);
		_delay_ms(500);
		
	
    }
}

//enabled
ISR(ADC_vect)
{
	
		
		//	medelvärde
		
		range = ADC_read();
		range_tot -= range_arr[i];	//remove oldest value from total
		range_arr[i] = range;		//and add read value in its place
		range_tot += range_arr[i];	//and add new value to total
		i = (i+1)%70;				//update i after interrupts
		
		if (i == 1)
		{
			FLIPBIT(PORTD, DDD6);
		}

}

//disabled
ISR(USART0_UDRE_vect)
{
	
}

ISR(USART0_RX_vect)
{	
	//ta emot data
	received_byte = usart_RX();
	
	if (received_byte == 'A')
	{
		FLIPBIT(PORTD, DDD4);
	}
}

ISR(USART0_TX_vect)
{

}