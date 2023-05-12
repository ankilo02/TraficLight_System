#ifndef _I2C_H_
#define _I2C_H_

#include <p18f4620.h>


#define WRITE		0
#define READ		1

#define ACK		0
#define N_ACK		1

#define TRIS_SCL	TRISCbits.RC3
#define TRIS_SDA	TRISCbits.RC4
#define SSPENB 		0x20
#define RTC_ADD		0xd0

unsigned char BCD2Dec(unsigned char BCD);
unsigned char Dec2BCD(unsigned char Dec);
void init_i2c(void);
void ds1307_control(unsigned char mRW_bit);
void eeprom_control(unsigned char mRW_bit);
void i2c_start(void);
void i2c_restart(void);
void i2c_stop(void);
void i2c_write(unsigned char abyte);
unsigned char i2c_read(void);
void i2c_ack(void);
void i2c_nak(void);
void write_ds1307(unsigned char address, unsigned char data);
unsigned char read_ds1307(unsigned char address);

void write_eeprom(unsigned int address, unsigned char data);
unsigned char read_eeprom(unsigned int address);

extern unsigned char flag_read,byte_num,index_add;
extern unsigned char receive_flag,ack_flag;
extern unsigned char i2c_data;
extern unsigned char ack_bit;

#endif