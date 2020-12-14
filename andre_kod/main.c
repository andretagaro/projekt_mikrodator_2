#include "Global_defs.h"
#include "INIT_functions.h"
#include "direction_functions.h"
#include "range_functions.h"

static int UARTputchar(char ch, FILE *stream);
static FILE mystream = FDEV_SETUP_STREAM(UARTputchar, NULL, _FDEV_SETUP_WRITE);

volatile uint8_t received_bytes[5] = {0};
volatile uint8_t receive_complete = 0;

int main(void)
{
	SETBIT(DDRC, 3);
	
	uint32_t distance_counter = 0;
	
	uint16_t adc_value = 0;
	uint16_t adc_array[10] = {0};
	uint8_t range_cm = 0;
	
	uint8_t left_right = 10;
	uint8_t forward_backwards = 10;

	PWM_INIT();
	UART_INIT();
	ADC_INIT();
	
	stdout = &mystream;
	
	sei();
	
    while (1) 
    {	
		if(distance_counter % 3000 == 0 )
		{
			adc_value = read_adc();
			adc_value = avarage_adc(adc_value, adc_array);
		}

		if(distance_counter == 30000)
		{
		range_cm = calculate_distance(adc_value);
		range_handler(range_cm);
		distance_counter = 0;
		}
		
		if(receive_complete == 1)
		{

			calculate_direction(received_bytes, &left_right, &forward_backwards);
			move_function(forward_backwards, left_right, range_cm);
			receive_complete = 0;

		}
	
		distance_counter++;
    }
}


ISR(USART0_RX_vect)
{
	uint8_t clear = 0;
	
	if(UDR0 == 'D')
	{
		received_bytes[0] = 'D';
		
		for(uint8_t i = 1; i < 5; i++)
		{
			while((UCSR0A & (1 << RXC0)) == 0)
			{
			}
			received_bytes[i] = UDR0;
				
		}
		
		receive_complete = 1;
	}
	else
	{
		clear = UDR0;
	}
}


static int UARTputchar(char ch, FILE *stream)
{
	while((UCSR0A & (1 << UDRE0)) == 0)
	{
	}
	UDR0 = ch;

	return 0;
}

