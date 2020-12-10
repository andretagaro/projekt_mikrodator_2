
#include "globaldefs.h"
#include "UART_util.h"
#include <avr/io.h>
#include <avr/interrupt.h>

void usart_send_string(unsigned char *str)
{
	while (*str > 0)
	{
		usart_TX(*str);
		str++;
	}
}


void usart_TX(unsigned char data)
{
	
	while (bit_is_clear(UCSR0A, UDRE0))
	{	}
	
	UDR0 = data;
}


unsigned char usart_RX(void)
{
	while (bit_is_clear(UCSR0A, RXC0))
	{	}
	
	return UDR0;
}


void usart_init(void)
{
	SETBIT (DDRD, 1);	//PD1 is transmitter output
	CLEARBIT (DDRD, 0); //PD0 is receiver input
	
	UBRR0  = BAUD_PRESCALE;
	UBRR0H = (BAUD_PRESCALE >> 8) ; // upper 8 bits into high byte
	UBRR0L = BAUD_PRESCALE ;		// lower 8 bits into low byte
	
	UCSR0A = 0x00;
	UCSR0B = (1<<TXEN0)	 | (1<<RXEN0); //enable transmitter & receiver
	UCSR0B |= (1<<RXCIE0) | (1<<TXCIE0);	//Receive complete, Transmit complete
	//UCSR0B |= (1<<UDRIE0);					//USART Data Register Empty
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);		// 8bit data, no parity, 1 stop bit

	sei();
}