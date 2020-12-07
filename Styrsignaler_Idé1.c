#define BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU / (BAUDRATE * 16UL))) - 1)
#define FLIPBIT(ADDR,BIT) (ADDR ^= (1<<BIT))
#define SETBIT(ADDR,BIT) (ADDR |= (1<<BIT))
#define CLEARBIT(ADDR,BIT) (ADDR &= ~(1<<BIT))
#define BIT_IS_CLEAR(ADDR,BIT) (!(ADDR & (1<<BIT)))

/*********************
PWM-initerering jag testade move_function med
**********************/

void PWM_INIT(void)
{
	SETBIT(DDRB, 3); // PWM0, A
	SETBIT(DDRB, 4); // PWM0, B
	SETBIT(DDRD, 6); //PWM2, B
	SETBIT(DDRD, 7); //PWM2, A
	
	//AKTIVERA PWM0 PHASE-CORRECT-MODE
	TCCR0A = (1 << WGM00);
	TCCR0B = (1 << CS02) | (1 << CS00); // Prescaler 1024
	//AKTIVERA PWM2 PHASE-CORRECT-MODE
	TCCR2A = (1 << WGM20);
	TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20); //Prescaler 1024
}

/************************************************************************/
/*Funktionen fungerar enligt följande: Om forward_backwards är högre än 10
så ges PWM-signaler på de pinnar som ger motorerna kraft framåt.
Om talet är mindre än 10 så ges ges PWM-signaler på de pinnar som ger motorerna kraft bakåt.

För att bestämma åt vilket håll vagnen ska åka åt så dras värdet left_right av från respektive motors duty.

exempel:

Första talet anger styrka, andra talet anger riktning.
10 anger centralt på joysticken.

1010, ge ingen kraft till motorerna
2020, ge full kraft åt båda motorena framåt
0000, ge full kraft åt båda motorena bakåt
1505, ge medelkraft åt motorena men mer kraft åt höger motor
1505, ge medelkraft åt motorena men mer kraft åt höger motor

*/
/************************************************************************/
void move_function(uint8_t forward_backwards, uint8_t left_right)
{
	// Talet max_limit bestämmer motorernas maxduty. 15 ger en maxduty på (15*10/255)*100%
	uint8_t max_limit = 10;
	
	if(forward_backwards > 10)
	{
		
		forward_backwards = forward_backwards - 10;
		
		if(left_right > 10)
		{
			left_right = left_right - 10;
			OCR0A = max_limit * forward_backwards;
			OCR2A = max_limit * (forward_backwards - left_right);
		}
		else if(left_right < 10)
		{
			left_right = 10 - left_right;
			OCR0A = max_limit * (forward_backwards - left_right);
			OCR2A = max_limit * forward_backwards;
		}
		else
		{
			OCR0A = max_limit * forward_backwards;
			OCR2A = max_limit * forward_backwards;
		}
		
		
		CLEARBIT(TCCR0A, COM0B1);
		CLEARBIT(TCCR2A, COM2B1);
		
		SETBIT(TCCR0A, COM0A1);
		SETBIT(TCCR2A, COM2A1);
		
	}
	else if(forward_backwards < 10) //bakåt
	{
		
		forward_backwards = 10 - forward_backwards;
		
		if(left_right > 10)
		{	left_right = left_right - 10;
			OCR0B = max_limit * forward_backwards;
			OCR2B = max_limit * (forward_backwards - left_right);
		}
		else if(left_right < 10)
		{
			left_right = 10 - left_right;
			OCR0B = max_limit * (forward_backwards - left_right);
			OCR2B = max_limit * forward_backwards;
		}
		else
		{
			OCR0B = max_limit * forward_backwards;
			OCR2B = max_limit * forward_backwards;
		}
		
		CLEARBIT(TCCR0A, COM0A1);
		CLEARBIT(TCCR2A, COM2A1);
		
		SETBIT(TCCR0A, COM0B1);
		SETBIT(TCCR2A, COM2B1);
		
		
	}
	else
	{
		OCR0A = 0;
		OCR2A = 0;
		OCR0B = 0;
		OCR2B = 0;
		
		CLEARBIT(TCCR0A, COM0B1);
		CLEARBIT(TCCR2A, COM2B1);
		CLEARBIT(TCCR0A, COM0A1);
		CLEARBIT(TCCR2A, COM2A1);
		
	}
}


/************************************************************************/
/* UART-init för att testa UARTgetdata                                  */
/************************************************************************/

void UART_INIT(void)
{
	SETBIT(DDRD, 1); // TX = OUT
	CLEARBIT(DDRD, 0); // RX = IN

	UBRR0H = (BAUD_PRESCALE >> 8);
	UBRR0L = BAUD_PRESCALE;

	UCSR0A = 0x00;
	UCSR0B = (1<<RXEN0)|(1<<TXEN0); //aktivera RX, TX
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00); // 8 data 1 stopp
}


/************************************************************************/
/* Hämta fyra värden från UART                                          */
/************************************************************************/

void UARTgetdata(uint8_t *forward_backwards_P, uint8_t *left_right_P)
{
	uint8_t received_bytes[4] = {0};
	
	for(uint8_t i = 0; i < 4; i ++)
	{
		while((UCSR0A & (1 << RXC0)) == 0)
		{
		}
		
		received_bytes[i] = UDR0;
	}
	
	//Detta omvandlar ascii-formatet till integers.
	received_bytes[0] -= 48;
	received_bytes[0] *= 10;
	received_bytes[1] -= 48;
	*forward_backwards_P = received_bytes[0] + received_bytes[1];
	
	received_bytes[2] -= 48;
	received_bytes[2] *= 10;
	received_bytes[3] -= 48;
	*left_right_P = received_bytes[2] + received_bytes[3];

}