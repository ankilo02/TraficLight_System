#include "output.h"

#define     ON      1
#define     OFF     0

#define     LED     PORTD
#define     LED2    PORTA
#define     LAT2    LATA
#define     LED3    PORTE

unsigned char arrayMapOfOutput [8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
unsigned char statusOutput[8] = {0,0,0,0,0,0,0,0};
unsigned char statusOutput2[8] = {0,0,0,0,0,0,0,0};

int portAo2=0;
int portAo2_length=6;
int portAo=0;
int portAo_length=4;
int portAo3=0;
int portAo3_length=7;

void OpenOutput(int index)
{
	if (index >= 0 && index <= 7)
	{
		LED = LED | arrayMapOfOutput[index];
	}

}

void CloseOutput(int index)
{
	if (index >= 0 && index <= 7)
	{
		LED = LED & ~arrayMapOfOutput[index];
	}
}

void ReverseOutput(int index)
{
    if (statusOutput[index]  == ON)
    {
        CloseOutput(index);
        statusOutput[index] = OFF;
    }
    else
    {
        OpenOutput(index);
        statusOutput[index] = ON;
    }
}

void OpenOutput2(int index)
{
	if (index >= 0 && index <= 7)
	{
		LED2 = LAT2 | arrayMapOfOutput[index];
	}

}

void CloseOutput2(int index)
{
	if (index >= 0 && index <= 7)
	{
		LED2 = LAT2 & ~arrayMapOfOutput[index];
	}
}

void ReverseOutput2(int index)
{
    if (((LAT2 >> index)&1)  == ON)
    {
        CloseOutput2(index);

    }
    else
    {
        OpenOutput2(index);
    }
}
void setPort2(int x) {
    LED2=(x&~0xFFC0)|(LAT2&0xFFC0);
}
void OpenOutputE()
{
		LED3 = LED3 | arrayMapOfOutput[0];

}

void CloseOutputE() {
    
    LED3 = LED3 & ~arrayMapOfOutput[0];
}
void sendData2(int data,int length) {
    int i=0;
    for (i=0;i<length;i++)
    {
        if (((data >> i)&1)  == ON) {OpenOutput2(0);} else {CloseOutput2(0);}
        OpenOutput2(1);
        CloseOutput2(1);
    }
    OpenOutput2(2);
    CloseOutput2(2);
    
}
void OpenOutputPortAo2(int index)
{
	if (index >= 0 && index <= 7)
	{
		portAo2 = portAo2 | arrayMapOfOutput[index];
	}
    sendData2(portAo2,portAo2_length);
}

void CloseOutputPortAo2(int index)
{
	if (index >= 0 && index <= 7)
	{
		portAo2 = portAo2 & ~arrayMapOfOutput[index];
	}
     sendData2(portAo2,portAo2_length);
}

void setPortAo2(int x) {
    portAo2=(x&~0xFFC0)|(portAo2&0xFFC0);
    sendData2(portAo2,portAo2_length);
}

void Flash_yellow() {
    if (((portAo2 >> 1)&1)  == ON)
    {
        CloseOutputPortAo2(1);
        CloseOutputPortAo2(4);
        UartSendString("!LIGHT:2:5#");
    }
    else
    {
        OpenOutputPortAo2(1);
        OpenOutputPortAo2(4);
        UartSendString("!LIGHT:7:7#");
    }
}

void ReverseOutputPortAo2(int index)
{
    if (((portAo2 >> index)&1)  == ON)
    {
        CloseOutputPortAo2(index);

    }
    else
    {
        OpenOutputPortAo2(index);
    }
}



void sendData(int data,int length) {
    int i=0;
    for (i=0;i<length;i++)
    {
        if (((data >> i)&1)  == ON) {OpenOutput2(3);} else {CloseOutput2(3);}
        OpenOutput2(4);
        CloseOutput2(4);
    }
    OpenOutput2(5);
    CloseOutput2(5);
    
}
void OpenOutputPortAo(int index)
{
	if (index >= 0 && index <= 7)
	{
		portAo = portAo | arrayMapOfOutput[index];
	}
    sendData(portAo,portAo_length);
}

void CloseOutputPortAo(int index)
{
	if (index >= 0 && index <= 7)
	{
		portAo = portAo & ~arrayMapOfOutput[index];
	}
     sendData(portAo,portAo_length);
}

void setPortAo(int x) {
    portAo=(x&~0xFFC0)|(portAo&0xFFC0);
    sendData(portAo,portAo_length);
}

void ReverseOutputPortAo(int index)
{
    if (((portAo >> index)&1)  == ON)
    {
        CloseOutputPortAo(index);

    }
    else
    {
        OpenOutputPortAo(index);
    }
}

void sendData3(int data,int length) {
    int i=0;
    for (i=0;i<length;i++)
    {
        if (((data >> i)&1)  == ON) {OpenOutput(0);} else {CloseOutput(0);}
        OpenOutput(1);
        CloseOutput(1);
    }
    OpenOutput(2);
    CloseOutput(2);
    
}
void OpenOutputPortAo3(int index)
{
	if (index >= 0 && index <= 7)
	{
		portAo3 = portAo3 | arrayMapOfOutput[index];
	}
    sendData3(portAo3,portAo3_length);
}

void CloseOutputPortAo3(int index)
{
	if (index >= 0 && index <= 7)
	{
		portAo3 = portAo3 & ~arrayMapOfOutput[index];
	}
     sendData3(portAo3,portAo3_length);
}

void setPortAo3(int x) {
    portAo3=(x&~0xFFC0)|(portAo3&0xFFC0);
    sendData3(portAo3,portAo3_length);
}

void ReverseOutputPortAo3(int index)
{
    if (((portAo3 >> index)&1)  == ON)
    {
        CloseOutputPortAo3(index);

    }
    else
    {
        OpenOutputPortAo3(index);
    }
}