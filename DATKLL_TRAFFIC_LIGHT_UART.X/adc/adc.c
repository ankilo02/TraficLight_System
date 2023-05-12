#include "adc.h"
unsigned int adc_value[ADC_CHANNEL];
void init_adc(void)
{
	TRISA = 0x01;
	ADCON1 = 0x0e;
	ADCON0 = 0x00;
	ADCON2 = 0x04;
	ADCON0bits.ADON = 1;
}

int get_adc_value()
{
	int result = 0;
	ADCON0bits.GO_DONE = 1;
	while(ADCON0bits.GO_DONE ==1){}
	result = ADRESH;
	result = (result<<2) + (ADRESL>>6);
        adc_value[0] = result;
	return result;
}

