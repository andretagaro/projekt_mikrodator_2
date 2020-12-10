
#include "globaldefs.h"
#include "UART_util.h"

void send_in_ASCII(unsigned int heltal)
{
	
	_Bool is_sent = 0;
	unsigned char number = 0;

	while (heltal >= 1000)
	{
		number++;
		heltal -= 1000;
	}
	
	if (number > 0)
	{
		usart_TX((number+48));
		is_sent = 1;
	}
	
	
	number = 0;
	while (heltal >= 100)
	{
		number++;
		heltal -= 100;
	}
		
	if ((number > 0) | (is_sent == 1))
	{
		usart_TX((number+48));
		is_sent = 1;
	}
		
		
	number = 0;
	while (heltal >= 10)
	{
		number++;
		heltal -= 10;
		
	}
			
	if ((number > 0) | (is_sent == 1))
	{
		usart_TX((number+48));
		is_sent = 1;
	}
			
			
	number = 0;
	while (heltal >= 1)
	{
		number++;
		heltal -= 1;
	}
				
	usart_TX((number+48));
	is_sent = 1;
	
	
	usart_TX('\n');
}