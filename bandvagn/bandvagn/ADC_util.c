#include <avr/interrupt.h>

unsigned int result;

void init_ADC()
{
	PORTA = 0;
	DDRA  = 0; //in
	
	ADCSRA |= (1<<ADEN);						//enable ADC
	ADCSRA |= (1<<ADIE);						//ADC Conversion Complete interrupt enable
	ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); //prescaler/division factor 128
	ADMUX   = (1<<MUX1) |(1 << REFS1) | (1 << REFS0);	// Left adjust result, Internal 2.56V Voltage Reference, input channel = ADC0 (mux3_0 = 0000)
	// (1<<ADLAR) | 
	sei();
}

unsigned int ADC_single_read()
{
	ADCSRA	|= (1<<ADSC);	//start reading
	
	while((ADCSRA & (1<< ADIF))==0)
	{	/* wait for conversion */ }
	
	
	result = ADC;

	return result;
}

unsigned int ADC_read()
{
	result = ADC;

	// not needed when using one channel
	//ADMUX	=  (1<<channel) | (1 << REFS1) | (1 << REFS0);		//Internal 2.56V Voltage Reference, input channel = ADC0 (mux3_0 = 0000)
	//PRR0	&= (0<<PRADC);	
	
	ADCSRA	|= (1<<ADSC);	//start reading

	return result;
}


