/******************************
* Vref = 2,56 V               *
******************************/
uint8_t calculate_distance(uint16_t adc_value)
{
	uint8_t range_cm = 0;
	
	if(adc_value >= 55 && adc_value < 65)
	{range_cm = 50;}
	else if(adc_value >= 65 && adc_value < 80)
	{range_cm = 45;}
	else if(adc_value >= 80 && adc_value < 96)
	{range_cm = 40;}
	else if(adc_value >= 96 && adc_value < 123 )
	{range_cm = 35;}
	else if(adc_value >= 123 && adc_value < 163 )
	{range_cm = 30;}
	else if(adc_value >= 163 && adc_value < 210 )
	{range_cm = 25;}
	else if(adc_value >= 210 && adc_value < 308)
	{range_cm = 20;}
	else if(adc_value >=  308 && adc_value < 430 )
	{range_cm = 15;}
	else if(adc_value >= 430 && adc_value < 450)
	{range_cm = 10;}
	else if(adc_value >= 450 && adc_value < 530)
	{range_cm = 9;}
	else if(adc_value >=  530 && adc_value < 590)
	{range_cm = 8;}
	else if(adc_value >= 590 && adc_value < 700 )
	{range_cm = 7;}
	else if(adc_value >= 700 && adc_value < 800)
	{range_cm = 6;}
	else if(adc_value >=  800 && adc_value < 900)
	{range_cm = 5;}
	else if(adc_value >=  900 && adc_value < 1020 )
	{range_cm = 4;}
	else if(adc_value >= 1020)
	{range_cm = 3;}
	else range_cm = 255; // använder detta som en indikator på att vi inte har något i närheten.
									
	return range_cm;
}
