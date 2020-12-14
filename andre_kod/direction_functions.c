#include "direction_functions.h"

/************************************************************************/
/*Funktionen fungerar enligt f�ljande: Om forward_backwards �r h�gre �n 10
s� ges PWM-signaler p� de pinnar som ger motorerna kraft fram�t.
Om talet �r mindre �n 10 s� ges ges PWM-signaler p� de pinnar som ger motorerna kraft bak�t.

F�r att best�mma �t vilket h�ll vagnen ska �ka �t s� dras v�rdet left_right av fr�n respektive motors duty.

exempel:

F�rsta talet anger styrka, andra talet anger riktning.
10 anger centralt p� joysticken.

1010, ge ingen kraft till motorerna
2020, ge full kraft �t b�da motorena fram�t
0000, ge full kraft �t b�da motorena bak�t
1505, ge medelkraft �t motorena men mer kraft �t h�ger motor
1505, ge medelkraft �t motorena men mer kraft �t h�ger motor

*/
/************************************************************************/

void move_function(uint8_t forward_backwards, uint8_t left_right, uint8_t range_cm)
{
	// Talet max_limit best�mmer motorernas maxduty. 15 ger en maxduty p� (15*10/255)*100%
	uint8_t max_limit;
	
	if(range_cm == 3)
	{
		max_limit = 0;
	}
	else if(range_cm < 10 && range_cm > 3)
	{
		max_limit = 7;
	}
	else
	{
		max_limit = 15;
	}
	
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
	else if(forward_backwards < 10) //bak�t
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


void calculate_direction(volatile uint8_t received_bytes[], uint8_t *left_right_P, uint8_t *forward_backwards_P)
{
	//Detta omvandlar ascii-formatet till integers.
	received_bytes[1] -= 48;
	received_bytes[1] *= 10;
	received_bytes[2] -= 48;
	*forward_backwards_P = received_bytes[1] + received_bytes[2];
	
	received_bytes[3] -= 48;
	received_bytes[3] *= 10;
	received_bytes[4] -= 48;
	*left_right_P = received_bytes[3] + received_bytes[4];
}