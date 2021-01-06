/*
//********************************************

To use ssd1306 128x64 OLED display on atmega1284p 
couple of libraries where downloaded and
included in the main project. These necessary libraries can 
be found on this website:
https://bintray.com/olikraus/u8glib/AVR 

A tutorial on how to initiate the screen can be found on github:
https://github.com/olikraus/u8glib/wiki/avr

*/
//**********************************************

#define F_CPU 8000000UL
#include <avr/io.h>
#include "u8g.h"
#include <stdlib.h>



uint8_t a_data[8] = {0};
volatile uint8_t b_data[8] = {0};

volatile uint8_t i = 0;
volatile uint8_t data_ascii = 0;
volatile uint8_t test_data[8] = {0};

void draw(void) 
{
	char rx_0[10];
	char rx_1[10];
	char rx_2[10]; 

	//snprintf convert and stores a variable in a buffer   
	snprintf(rx_0, 10, "%d", a_data[0]);  // First position of array contain a letter 
	snprintf(rx_1, 10, "%d", (a_data[1] - 48));  // Get none ascii number 
	snprintf(rx_2, 10, "%d", (a_data[2] - 48));
	
	
	u8g_SetFont(&u8g, u8g_font_10x20);	// Set screen font (letter size)
	u8g_DrawStr(&u8g, 0, 40, "Data:");	// Draw the string on oled-display
	u8g_DrawStr(&u8g, 50, 40, rx_0);
	u8g_DrawStr(&u8g, 60, 40, rx_1);
	u8g_DrawStr(&u8g, 70, 40, rx_2);
	
}



int main(void)
{
	CLKPR = 0x80; // Set clock speed, should be equal to system clock.
	CLKPR = 0x00;

	/*
     CS: PORTB, Bit 2 --> PN(1,4)
     A0: PORTD, Bit 7 --> PN(3,1)
     SCK: PORTB, Bit 5 --> PN(1,7)
     MOSI: PORTB, Bit 3 --> PN(1,5)
   */
//Make sure pin configuration on MCU is correctly connected to OLED
	u8g_InitSPI(&u8g, &u8g_dev_ssd1306_128x64_sw_spi, PN(1, 7), PN(1, 5), PN(1, 4), PN(3, 7), U8G_PIN_NONE);

    while (1) 
    {
	
		u8g_FirstPage(&u8g);
		do
		{
			draw();
		} while ( u8g_NextPage(&u8g) );
		u8g_Delay(50);
		
}

//USART interrupt routine 
// Data coming from serialport will end with character'\n'
//ASCII character for '\n' = 10
ISR(USART0_RX_vect)
{
	data_ascii = UDR0;

	//Check for '\n' at the end of string 
	if (data_ascii != 10) {
		test_data[i] = data_ascii;
		i++;
	}
	else if (data_ascii == 10) {			//when end of string is reached
		if (test_data[0] == 65) {		// Check for letter A = 65 (in ascii)
			memcpy(a_data, test_data, 8); 	// Copy received data to a new array
	
		} 
		else if (test_data[0] == 66)	{	// Check for letter B = 66 (in ascii)
			memcpy(b_data, test_data, 8); // Copy received data to new array
		
		}
		i = 0;
	}
}
