

/*
 * OLED_display.c
 *
 * Created: 2020-12-11 16:04:03
 * Author : kadeko
 */ 

#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <avr/wdt.h>
#include "u8g.h"
#include "initiation_files.h"
 
// Baud rate set and calculation
#define USART_BAUDRATE 9600
#define BAUD_PRESCALE ((((F_CPU / 16) + (USART_BAUDRATE / 2)) /(USART_BAUDRATE)) - 1)
#define SETBIT(ADDR, BIT) (ADDR |= (1 << BIT))
#define CLEARBIT(ADDR, BIT) (ADDR &= ~(1<<BIT))

// Function declaration
void uart_transmit(uint8_t data);
uint8_t uart_receive(void);
void uart_print_send(char ch);

void reset_prog(void);
void reset_prog_oled(void);

void draw_oled(void);

void tutorial_message(void);
void draw_tutorial(void);

void get_range(void);
void range_to_oled(void); 
void get_range_b(void);
void range_b_oled(void);

void no_connection(void);
void no_connection_oled(void);

uint8_t choice_carA(uint8_t move, uint8_t set);
uint8_t choice_carB(uint8_t move_b, uint8_t set_b);  

//Function to use printf to write to LCD-display
static uint8_t LCD_printChar(char ch, FILE *stream);
static FILE aurt_stream = FDEV_SETUP_STREAM(LCD_printChar, NULL, _FDEV_SETUP_WRITE);

static uint8_t LCD_printChar(char ch, FILE *stream)
{
	uart_transmit(ch);

	return 0;
}

//Watch-dog section
uint8_t mcusr_mirror __attribute__((section (".noinit")));

void wdt_init(void) __attribute__((naked))__attribute__((section(".init3")));

void wdt_init(void) 
{
	mcusr_mirror = MCUSR; //Save the value in MCUSR
	MCUSR = 0; //
	wdt_disable();  // Disable interrupts
	wdt_reset();    //Reset Watchdog timer
	//Start timed sequence
	WDTCSR |= (1 << WDCE)|(1 << WDE); 
	//Set prescaler and desired cycles
	WDTCSR = (1 << WDE )|(1 << WDP3)|(1<<WDP0);
	
	 
}

//Global and volatile variable declaration
volatile uint16_t x_sig = 0;
volatile uint16_t y_sig = 0;

volatile uint8_t i = 0;
uint8_t size = 0;
uint8_t a_data[4] = {0};
uint8_t b_data[4] = {0};
volatile uint8_t test_data[4] = {0};
volatile uint8_t var_set = 0;

	
u8g_t u8g;


int main(void)
{
	
	uint8_t move_left = 0;
	uint8_t move_right = 0;
	uint8_t set_x = 0;
	
	CLKPR = 0x80; // Set clock speed, should be equal system clock.
	CLKPR = 0x00;
	sei();
	uart_init(BAUD_PRESCALE);
	spi_init();
	u8g_InitSPI(&u8g, &u8g_dev_ssd1306_128x64_sw_spi, PN(1, 7), 
				PN(1, 5), PN(1, 4), PN(3, 7), PN(1, 3));
	
	
	draw_tutorial();
	_delay_ms(3500);

	CLEARBIT(DDRA, 7);	//Set pin7 as an input
	SETBIT(PORTA, 7);	//Activate pull-up
	
	

    while (1) 
    {
		wdt_reset();		//Reset watchdog timer
		
		if (y_sig > 10) {	// Infinity loop that waits for watchdog timer-out to init 
			while(1) {
			reset_prog_oled();
			}
		}
		draw_oled();
		x_sig = adc_read(6);
		y_sig = adc_read(5);
		
		if(x_sig < 8) {
			move_left = 1; 
		}
		if(x_sig > 12) {
			move_right = 1;
		}
		
		move_left =  choice_carA(move_left, set_x);
		move_right = choice_carB(move_right, set_x);
		
		wdt_reset();		//	Reset watchdog
    }
}

void uart_transmit(uint8_t data)
{

	/* Wait for empty transmission buffer*/
	while (( UCSR0A & (1<<UDRE0)) == 0)
	;
	/* Put data into buffer and send the data.*/
	UDR0 = data;
	
}

void uart_print_send(char c)
{
	stdout = &aurt_stream;
	printf("%c%.2d%.2d\n", c, x_sig, y_sig);
	
}

ISR(USART0_RX_vect)
{  
	uint8_t data_ascii = 0;
	data_ascii = UDR0;

	if (data_ascii != 10) {
		test_data[i] = data_ascii;
		i++;						
		}else if (data_ascii == 10) {
							
			if (test_data[0] == 65) {
			
				memcpy(a_data, test_data, 4); // Copy received data to new array
				memset(test_data, 48 , 4);
	
			} 
			else if (test_data[0] == 66)	{
				
				memcpy(b_data, test_data, 4); // Copy received data to new array
		
			
			}
			
			else if (test_data[0] == 67) {
				a_data[0] = test_data[0];
			}
			
			else if (test_data[0] == 68) {
				b_data[0] = test_data[0] ;
			}
			
			
			i = 0; // Set i = 0 to prepare for new data to be stored in test_data array.
	}
}

void reset_prog(void)
{
	u8g_SetFont(&u8g, u8g_font_8x13);	// Set screen font (letter size)
	u8g_DrawStr(&u8g, 0, 40, "Restart init...");
	
	
}

void reset_prog_oled(void)
{
	u8g_FirstPage(&u8g);
	do
	{
		reset_prog();
	} while ( u8g_NextPage(&u8g) );
	u8g_Delay(100);
}

void draw(void)
{
	uint8_t A = 65;
	uint8_t B = 66;
	 
	u8g_SetFont(&u8g, u8g_font_8x13);	// Set screen font (letter size)
	u8g_DrawStr(&u8g, 0, 15, "Main: Remote Car");
	u8g_DrawStr(&u8g, 0, 35, "   CarA:");
	u8g_DrawStr(&u8g, 0, 55, "   CarB:");
	
	if ((a_data[0] != A) && (b_data[0] != B)) {
		u8g_DrawStr(&u8g, 50, 35, "  off");
		u8g_DrawStr(&u8g, 50, 55, "  off");
	}
	else if ((a_data[0] == A) && (b_data[0] != B)) {
		u8g_DrawStr(&u8g, 50, 35, "  on");
		u8g_DrawStr(&u8g, 50, 55, "  off");
	}
	else if ((a_data[0] != A) && (b_data[0] == B)) {
		u8g_DrawStr(&u8g, 50, 30, "  off");
		u8g_DrawStr(&u8g, 50, 55, "  on");
	}
	else if ((a_data[0] == A) && (b_data[0] == B)) {
		u8g_DrawStr(&u8g, 50, 35, "  on");
		u8g_DrawStr(&u8g, 50, 55, "  on");
	}
	
}

void draw_oled(void) 
{
	u8g_FirstPage(&u8g);
	do
	{
		draw();
	} while ( u8g_NextPage(&u8g) );
	u8g_Delay(100);
}

void tutorial_message(void) 
{
	u8g_SetFont(&u8g, u8g_font_8x13);	// Set screen font (letter size)
	u8g_DrawStr(&u8g, 0, 12, "Move joystick");
	u8g_SetFont(&u8g, u8g_font_7x13);
	u8g_DrawStr(&u8g, 0, 35, "Right -->  CarA");
	u8g_DrawStr(&u8g, 0, 50, "Left  -->	 CarB");
	
}

void draw_tutorial(void)
{
	u8g_FirstPage(&u8g);
	do
	{
		tutorial_message();
	} while ( u8g_NextPage(&u8g) );
	u8g_Delay(100);
}


void get_range(void)
{
	char s_str_y[9];
	char s_str_x[9];
	
	char rx_0[10];
	char rx_1[10];
	char rx_2[10];
	char rx_3[10];
	
	snprintf(rx_0, 10, "%c", a_data[0]); // Convert ASCII digit to char
	snprintf(rx_1, 10, "%d", (a_data[1] - 48)); // Convert int to string
	snprintf(rx_2, 10, "%d", (a_data[2] - 48));
	snprintf(rx_3, 10, "%d", (a_data[3] - 48));
	
	/*if(a_data[1] != 0) {
		snprintf(rx_1, 10, "%d", (a_data[1] - 48)); // Convert int to string
	}*/
	
	snprintf(s_str_x, 9, "%d", x_sig); //Convert an int to a string
	snprintf(s_str_y, 9, "%d", y_sig);
	
	u8g_SetFont(&u8g, u8g_font_8x13);	// Set screen font (letter size)
	u8g_DrawStr(&u8g, 0, 12, "Car A is ready");
	u8g_DrawStr(&u8g, 0, 30, "Range:");		//Set position (where to print) and print to OLED.
	u8g_DrawStr(&u8g, 50, 30, rx_1);
	u8g_DrawStr(&u8g, 60, 30, rx_2);
	u8g_DrawStr(&u8g, 70, 30, rx_3);
	u8g_DrawStr(&u8g, 80, 30, "cm");
	
	u8g_DrawStr(&u8g, 0, 45, "move:A");
	u8g_DrawStr(&u8g, 50, 45, s_str_y);
	u8g_DrawStr(&u8g, 70, 45, s_str_x);
	u8g_SetFont(&u8g, u8g_font_6x10);
	u8g_DrawStr(&u8g, 0, 62, "Press down to return");
}


void range_to_oled(void) 
{
	u8g_FirstPage(&u8g);
	do
	{
		get_range();
	} while ( u8g_NextPage(&u8g) );
	u8g_Delay(100);
	
}

void get_range_b(void)
{
	char s_str_y[9];
	char s_str_x[9];
	
	char rx_0[10];
	char rx_1[10];
	char rx_2[10];
	char rx_3[10];
	
	snprintf(rx_0, 10, "%c", b_data[0]); // Convert ASCII digit to char
	snprintf(rx_1, 10, "%d", (b_data[1] - 48)); // Convert int to from ascii and then to string
	snprintf(rx_2, 10, "%d", (b_data[2] - 48));
	snprintf(rx_3, 10, "%d", (b_data[3] - 48));
	
	snprintf(s_str_x, 9, "%d", x_sig); //Convert an int to a string
	snprintf(s_str_y, 9, "%d", y_sig);
	
	u8g_SetFont(&u8g, u8g_font_8x13);	// Set screen font (letter size)
	u8g_DrawStr(&u8g, 0, 12, "Car B is ready");		
	u8g_DrawStr(&u8g, 0, 30, "Range:");		//Set position (where to print) and print to OLED.
	u8g_DrawStr(&u8g, 50, 30, rx_1);
	u8g_DrawStr(&u8g, 60, 30, rx_2);
	u8g_DrawStr(&u8g, 80, 30, "cm");
	
	u8g_DrawStr(&u8g, 0, 45, "move:B");
	u8g_DrawStr(&u8g, 50, 45, s_str_y);
	u8g_DrawStr(&u8g, 70, 45, s_str_x);
	u8g_SetFont(&u8g, u8g_font_6x10);
	u8g_DrawStr(&u8g, 0, 60, "Press down to return");
	
}

void range_b_oled(void)
{
	u8g_FirstPage(&u8g);
	do
	{
		get_range_b();
	} while ( u8g_NextPage(&u8g) );
	u8g_Delay(100);
	
}

uint8_t choice_carA(uint8_t move, uint8_t set)
{
	
	while(move == 1) {
		wdt_reset();
		//ADC-conversion function
		x_sig = adc_read(6);
		y_sig = adc_read(5);
		
		if (a_data[0] != 65) {
			no_connection_oled();
		} else 	{
			range_to_oled();
		
			if((x_sig != 10) || (y_sig != 10)) 	{
				uart_print_send('A');
				set = 1;
				}else {
					if (set == 1) {
						uart_print_send('A');
						set = 0;
					}
				
				}
			
			}
			if((PINA & (1 << PINA7)) == 0) {
				move = 0;
		}
	}
	return move;
}

uint8_t choice_carB(uint8_t move_b, uint8_t set_b)  
{
	
	while(move_b == 1) {
		wdt_reset();		//Reset watchdog timer
		
		//ADC-conversion function
		x_sig = adc_read(6);
		y_sig = adc_read(5);
		
		if (b_data[0] != 66) {
			no_connection_oled();
			} else 	{			
				range_b_oled();
				
			
			if((x_sig != 10) || (y_sig != 10)) 	{
				uart_print_send('B');
				set_b = 1;
				}else {
				if (set_b == 1) {
					uart_print_send('B');
					set_b = 0;
				}
				
			}
			
		}
		if((PINA & (1 << PINA7)) == 0) {
			move_b = 0;
		}
	}
	return move_b;
}

void no_connection(void) 
{
	u8g_SetFont(&u8g, u8g_font_8x13);	// Set screen font (letter size)
	u8g_DrawStr(&u8g, 30, 15, "This car");
	u8g_DrawStr(&u8g, 10, 30, "is not online");
	u8g_SetFont(&u8g, u8g_font_6x10);
	u8g_DrawStr(&u8g, 0, 55, "Press down to return");

}

void no_connection_oled(void)
{
	u8g_FirstPage(&u8g);
	do
	{
		no_connection();
	} while ( u8g_NextPage(&u8g) );
	u8g_Delay(100);
}




