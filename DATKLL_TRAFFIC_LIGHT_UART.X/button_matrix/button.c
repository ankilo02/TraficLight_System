#include "button.h"

unsigned int key_code[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//unsigned char arrayMaskOutputOfKey [8] = {0x80,0x40,0x20,0x10,0x01,0x02,0x04,0x08};
//unsigned char arrayMaskInputOfKey [8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
unsigned char arrayMaskOutputOfKey [4] = {0x10,0x20,0x40,0x80};
unsigned char arrayMaskInputOfKey [4] = {0x01,0x02,0x04,0x08};
unsigned char arrayMaskInputOfButton [8] = {0x01,0x02,0x04,0x08,
                                            0x10,0x20,0x40,0x80};

void init_key_matrix()
{
    //khoi tao nut nhan truc tiep
//	TRIS_BUTTON = 0xff;
//	PORT_BUTTON = 0xff;
    //khoi tao nut nhan cho ma tran phim
    TRIS_BUTTON = 0x0f;
	PORT_BUTTON = 0xff;
}

void scan_key_button()
{
	int i,j;
	for(i=0;i<8;i++)
	{
        if((PORT_BUTTON & arrayMaskInputOfButton[i]) == 0)
            key_code[i] = key_code[i] + 1;
        else
            key_code[i] = 0;
		
	}
}
void scan_key_matrix()
{
	int i,j;
	for(i=0;i<MAX_ROW;i++)
	{
		PORT_BUTTON = ~arrayMaskOutputOfKey[i];
		for(j=0;j<MAX_COL;j++)
		{
			if((PORT_BUTTON & arrayMaskInputOfKey[j]) == 0)
				key_code[i*MAX_ROW+j] = key_code[i*MAX_ROW+j] + 1;
			else
				key_code[i*MAX_ROW+j] = 0;
		}
	}
}

//ham nay de giup cac ban hieu ro viec quet ma tran phim
//Sau khi da hieu ve cach quet ma tran phim thi xoa di, hoac luu lai o mot file khac
void button_delay_ms(int value)
{
	int i,j;
	for(i=0;i<value;i++)
		for(j=0;j<238;j++);
}

void scan_key_matrix_demo() 
{
	int i,j;
	for(i=0;i<MAX_ROW;i++)     
	{
		PORT_BUTTON = ~arrayMaskOutputOfKey[i];
		for(j=0;j<MAX_COL;j++)
		{ 
			key_code[i*MAX_ROW+j] = 0;   
			if((PORT_BUTTON & arrayMaskInputOfKey[j]) == 0)  
			{
				key_code[i*MAX_ROW+j] = 1;
			}
		}
		PORTB = PORT_BUTTON;
		button_delay_ms(1000);
	}
}