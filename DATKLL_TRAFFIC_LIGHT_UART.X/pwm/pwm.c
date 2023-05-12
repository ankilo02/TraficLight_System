#include "pwm.h"
#include "..\adc\adc.h"

unsigned char duty_cycle = 0;
unsigned char speed;

void init_pwm()
{
		T2CONbits.TMR2ON = 0;  //turn off timer2
		T2CONbits.T2CKPS0 = 0; 
		T2CONbits.T2CKPS1 = 1;  //prescaler 1:16

		PR2 = 155;      //PWM period 
		CCP1CONbits.DC1B0 = 0;  //PWM duty cycle
		CCP1CONbits.DC1B1 = 0;  //2 bits LSB 
		CCPR1L = 0x00;     //8bits MSB

		CCP1CONbits.P1M0 = 0;  //PWM single mode
		CCP1CONbits.P1M1 = 0; 
		CCP1CONbits.CCP1M0 = 0;  //select PWM function
		CCP1CONbits.CCP1M1 = 0; 
		CCP1CONbits.CCP1M2 = 1; 
		CCP1CONbits.CCP1M3 = 1;

		TRISCbits.RC2 = 0;  //config RC2 output
		T2CONbits.TMR2ON = 1;  //start timer2
} 


void set_DC_speed(unsigned char value)
{
	CCPR1L = value;
}