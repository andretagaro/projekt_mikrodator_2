#include "INIT_functions.h"


void ADC_INIT(void)
{
	ADC_DDR = 0x00;
	CLEARBIT(PRR0, PRADC);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADATE) | (1 << ADSC); // Enable + F_CPU div 64
	ADCSRB = 0;
	ADMUX = (1 << REFS1) | (1 << REFS0) | (0 << ADLAR); // Single end + Ref 2,56 V + V-just
	/* Rensa MUX-bitar som aldrig används */
	CLEARBIT(ADMUX, MUX0);
	CLEARBIT(ADMUX, MUX1);
	CLEARBIT(ADMUX, MUX2);
	CLEARBIT(ADMUX, MUX3);
	CLEARBIT(ADMUX, MUX4);
}



void button_INIT(void)
{
	SETBIT(DDRA, 0); // Fram
	SETBIT(DDRA, 1); // Bak
	SETBIT(DDRA, 2); // Höger
	SETBIT(DDRA, 3); // Vänster
	
	SETBIT(PORTA, 0);
	SETBIT(PORTA, 1);
	SETBIT(PORTA, 2);
	SETBIT(PORTA, 3);
}



void PWM_INIT(void)
{
	SETBIT(DDRB, 3); // PWM0, A
	SETBIT(DDRB, 4); // PWM0, B
	SETBIT(DDRD, 6); //PWM2, B
	SETBIT(DDRD, 7); //PWM2, A
	
	//AKTIVERA PWM0 PHASE-CORRECT-MODE
	TCCR0A = (1 << WGM00);
	TCCR0B = (1 << CS02) | (1 << CS00); // Prescaler 1024
	
	//AKTIVERA PWM1
	TCCR2A = (1 << WGM20);
	TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20); //Prescaler 1024
}


void UART_INIT(void)
{
	SETBIT(DDRD, 1); // TX = OUT
	CLEARBIT(DDRD, 0); // RX = IN

	UBRR0H = (BAUD_PRESCALE >> 8);
	UBRR0L = BAUD_PRESCALE;

	UCSR0A = 0x00;
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)| (1<<RXCIE0); //aktivera RX, TX
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00); // 8 data 1 stopp
}


