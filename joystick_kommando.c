

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

volatile uint16_t x_sig = 0;
volatile uint16_t y_sig = 0;

//Command to use printf to write to USART
static uint8_t LCD_printChar(char ch, FILE *stream);
static FILE usart_stream = FDEV_SETUP_STREAM(LCD_printChar, NULL, _FDEV_SETUP_WRITE);

static uint8_t LCD_printChar(char ch, FILE *stream)
{
  //USART transmit function
	uart_transmit(ch);

	return 0;
}


  // Divide ADC value by 50 to get a double-digit value
  x_sig = adc_read(6) / 50;
  y_sig = adc_read(5) / 50;



void uart_init(uint8_t baud_r)
{
	DDRD = (1 << PIND1); //Set transmit pin to output

	/* Enable receiver, transmitter and receiver-interrupt flag */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);

	/* Set frame format; 8 data, 2 stop bit */
	UCSR0C = (1<<UCSZ01)|(3<<UCSZ00);
	UBRR0H =  (baud_r >> 8);
	UBRR0L =  baud_r;

}

void uart_transmit(uint8_t data)
{
	/* Wait for empty transmission buffer*/
	while (!( UCSR0A & (1<<UDRE0)))
	;
	/* Put data into buffer and send the data.*/
	UDR0 = data;


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

	return adc_volts;
}


// Transmit 4 digit values over USART using printf function 
void uart_print_send(void)
{
	stdout = &usart_stream;
	printf("D%.2d%.2d\n", y_sig,x_sig);
}
