/*
 * initiation_files.c
 *
 * Created: 2020-12-19 16:38:37
 *  Author: kadeko
 */ 
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>

// SPI communication ports
#define DDRB_SPI DDRB
#define PORTB_SPI PORTB
#define PIN_MOSI 5
#define PIN_SCK 7
#define PIN_SS 4


void spi_init(void)
{
	DDRB_SPI = (1 << PIN_MOSI) | (1 << PIN_SCK) | (1 << PIN_SS)|(0 << 3);
	DDRD = (1 << 7);
	
}

void uart_init(uint8_t baud_r)
{
	DDRD = (1 << 1); //Set transmit pin to output
	
	/* Enable receiver, transmitter and receiver-interrupt flag */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1 << RXCIE0);
	
	/* Set frame format; 8 data, 2 stop bit */
	UCSR0C = (1<<UCSZ01)|(3<<UCSZ00);
	UBRR0H =  (baud_r >> 8);
	UBRR0L =  baud_r;
}

uint16_t adc_read(uint8_t val)
{
	// ADC enable and clock setting
	ADCSRA |= (1<<ADEN)|(1<<ADPS1)|(1<<ADPS0);
	
	// AVcc with external capacitor at AREF pin.
	ADMUX = (1 << REFS0);
	ADMUX |= val;  // Choose pin to read ADC value from
	
	// Initiate conversion
	ADCSRA |= (1 << ADSC); // Start
	while ((ADCSRA & (1 << ADIF)) == 0); // Wait to finish conversion
	
	uint16_t adc_volts = ADC;
	
	return adc_volts / 50;
}

