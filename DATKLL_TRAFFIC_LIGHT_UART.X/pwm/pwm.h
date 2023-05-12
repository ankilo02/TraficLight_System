#ifndef _PWM_H_
#define _PWM_H_

#include <p18f4620.h>

extern unsigned char speed;
extern unsigned char duty_cycle;

void init_pwm(void);
void set_DC_speed(unsigned char value);


#endif