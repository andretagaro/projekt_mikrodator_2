/*
 * Projekt.c
 *
 * Created: 2020-12-09 17:52:22
 * Author : pamel
 */ 

#include "includes.h"
#include <string.h>
#define SETBIT(ADDR, BIT) (ADDR |= (1 << BIT))

void usart_init(void);
void adc_init(void);
void usar_transmit(unsigned char data);
unsigned char usart_receive(void);

void draw(void);
static int LCDputchar(char ch, FILE *stream);
void print_directions(char c, uint16_t h, uint16_t v);
void check_buttons(void);

u8g_t u8g;

volatile uint16_t vertical_data = 0, horizontal_data = 0;


const char online[]="online", offline[]="offline";

char statusA[10], statusB[10], print_cm_ten[10], print_cm_one[10], selected_car[10];
char recived_bit[8] = {0}, cm_a[8], cm_b[8];
//char cm_a[8], cm_b[8];

static FILE mystream = FDEV_SETUP_STREAM(LCDputchar, NULL, _FDEV_SETUP_WRITE);
volatile uint8_t digit = 0, choice = 0;
int main(void)
{
  
   
  //select minimal prescaler (max system speed) 
  CLKPR = 0x03;//Div by 8
  CLKPR = 0x00;
  
  /*
    CS: PORTB, Bit 4 --> PN(1,4)
    A0: PORTB, Bit 0 --> PN(1,3)
    SCK: PORTB, Bit 7 --> PN(1,7)
    MOSI: PORTB, Bit 5 --> PN(1,5)
	Reset: PORTB, Bit 2 --> PN(1,2)
  */

	
  DDRC = 0x00;
  PORTC = 0b00000011;
  stdout = &mystream;
        
  u8g_InitSPI(&u8g, &u8g_dev_ssd1306_128x32_sw_spi, PN(1, 7), PN(1, 5), PN(1, 4), PN(1, 3), PN(1,2));
  
  usart_init();
  adc_init();
   
  while(1){  
	  
		check_buttons();

	
	/*_delay_ms(50);
	ADCSRA |= (1 << ADSC);*/
		  		
	 
	
	 u8g_FirstPage(&u8g);
	 do
	 {
		 draw();
		  		
	  } while ( u8g_NextPage(&u8g) );
	  		 
	  		//u8g_Delay(100);
						
				
	 }
}

 
	
void usart_init(void){
	
	SETBIT(DDRD, 1);//TXD pin output & RX input
	
	UBRR0  = BAUD_PRESCALE;
	UBRR0H = (BAUD_PRESCALE >> 8) ; // upper 8 bits into high byte
	UBRR0L = BAUD_PRESCALE ;		// lower 8 bits into low byte
	
	UCSR0A = 0x00;
	UCSR0B = (1<<TXEN0) | (1<<RXEN0);//enable transmitter and receiver
	UCSR0B |= (1<<RXCIE0) ;	//Receive complete, Transmit complete
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);//8 bits data, no parity, 1 stop bit
	
}


void adc_init(void){
	
	PRR0 = (0 << PRADC);//Write '0' to PR ADC bit in the PRR
	ADCSRA = (1 << ADEN);//Write '1' to ADC enable bit in ADC C and SR A

	ADCSRA |= (1<< ADPS0) | (1 << ADPS1);// Divide clk by 8
	ADMUX = (1 << REFS0) | (0 << MUX1)| (0 << MUX0); //Set 2.56 V internal ref V and x-axis
	
	ADCSRA |= (1 << ADIE); //Interrupt enable

	ADCSRA |= (1 << ADSC);	//Write a '1' to ADC SC bit in ADC C and SR A
	//ADSC will be cleared when convo is complete
	sei();
}		


void usar_transmit(unsigned char data){
	//wait for empty transmit buffer
	while(bit_is_clear(UCSR0A, UDRE0)){
		//wait...
	}
	//put data into buffer, sends the data
	UDR0 = data;

}


unsigned char usart_receive(void){
	//wait for data to be received
	while(bit_is_clear(UCSR0A, RXC0)){
		//wait..
	}
	//get received data from buffer, and return
	return UDR0;
	
}


void draw(void)
{	
	u8g_SetFont(&u8g, u8g_font_6x10);
	
	if (choice == 1){	
		snprintf(print_cm_ten, 10, "%d", cm_a[0]);
		snprintf(print_cm_one, 10, "%d", cm_a[1]);
	}
	
	else if (choice == 2){
		snprintf(print_cm_ten, 10, "%d", cm_b[0]);
		snprintf(print_cm_one, 10, "%d", cm_b[1]);
	}
	
	
	u8g_DrawStr(&u8g, 0, 8, "Car A:");
	u8g_DrawStr(&u8g, 50, 8, statusA);
	u8g_DrawStr(&u8g, 0, 18, "Car B:");
	u8g_DrawStr(&u8g, 50, 18, statusB);
	u8g_DrawStr(&u8g, 0, 28, selected_car);
	u8g_DrawStr(&u8g, 10, 28, print_cm_ten);
	u8g_DrawStr(&u8g, 20, 28, print_cm_one);
	u8g_DrawStr(&u8g, 50, 28, "cm");
	


}


static int LCDputchar(char ch, FILE *stream){
	
	usar_transmit(ch);
	
	return 0;
}


void print_directions(char c, uint16_t h, uint16_t v){
	
		printf("\n%c%.2d%.2d", c, (h/50), (v/50));

}



void check_buttons(void){
	
	if (PINC == 0b00000010){//Car A
		choice = 1;
		strcpy(selected_car, "A ");
	}
	if (PINC == 0b00000001){//Car B
		choice = 2;
		strcpy(selected_car,"B ");
	}

}

ISR(USART0_RX_vect){

		if(usart_receive() == 'A'){
			
			strcpy(statusA, online);
							
			recived_bit[0] = usart_receive() ;
			recived_bit[1] = usart_receive() ;
			recived_bit[2] = usart_receive() ;
			printf("%c ", recived_bit[0]);
			printf("%c ", recived_bit[1]);
			printf("%c ", recived_bit[2]);
			//memcpy(cm_a, recived_bit, 2);
			

		}
		
				
		if(usart_receive() == 'B'){
			
			strcpy(statusB, online);
			
			recived_bit[0] = usart_receive() ;
			recived_bit[1] = usart_receive() ;
			
			printf("%d ", recived_bit[0]);
			printf("%d ", recived_bit[1]);
			
			memcpy(cm_b, recived_bit, 2);			


		}
		
		if(usart_receive() == 'C'){
			
			strcpy(statusA, offline);
		}
		
		
		if(usart_receive() == 'D'){
			
			strcpy(statusB, offline);

		}
		
	memset(recived_bit, 0, 8);
	
/*
int i = 0;

	if (usart_receive() != 'J'){
		
		recived_bit[i] = usart_receive();
		i++;
	}

	else if (usart_receive() == 'J'){
		
		if (recived_bit[0] == 'A'){//A
			strcpy(statusA, online);
		}
		if(recived_bit[0] == 'B'){//B
			strcpy(statusB, online);
		}
		if(recived_bit[0] == 'C'){//C
			strcpy(statusA, offline);
		}
	
		if(recived_bit[0]== 'D'){//D
			strcpy(statusB, offline);
		}*/

}

ISR(ADC_vect){
	
	if (digit == 0){ //horizontal data
		
		horizontal_data = ADC;
		
		digit = 1;
		
		ADMUX = (1 << REFS0) | (0 << MUX1) | (1 << MUX0);//enable horizontal_data input
	}
	
	else if (digit == 1){

		vertical_data = ADC;
		
		if (choice == 1){
			print_directions('A',horizontal_data, vertical_data);
		}
		else if (choice == 2){
			print_directions('B',horizontal_data, vertical_data);
		}
		
		
		digit = 0;
		
		ADMUX =  (1 << REFS0) | (0 << MUX1) | (0 << MUX0);//enable vertical_data input
	}
	
	
}
