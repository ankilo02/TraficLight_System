
#include "seg7.h"
#include "..\timer\timer.h"
#include "output.h"

#define NO_OF_LED_TO_SCAN 4


int portScan[4]={3,4,5,6};
int led_buffer[4]={1,2,4,8};
int led_index=0;
int NUM[11][7]={
{0,0,0,0,0,0,1},
{1,0,0,1,1,1,1},
{0,0,1,0,0,1,0},
{0,0,0,0,1,1,0},
{1,0,0,1,1,0,0},
{0,1,0,0,1,0,0},
{0,1,0,0,0,0,0},
{0,0,0,1,1,1,1},
{0,0,0,0,0,0,0},
{0,0,0,0,1,0,0},
{1,1,1,1,1,1,1},
};
void display7SEG(int num) // display cho led 7 ?o?n 1
{
    int i=0;
    int k=0;
   for(i=0;i<7;i++){
       switch (i) {
           
           case 0: k=2;
           break;
           case 1: k=3;
           break;
           case 2: k=6;
           break;
           case 3: k=5;
           break;
           case 4: k=4;
           break;
           case 5: k=1;
           break;
           case 6: k=0;
           break;

           default: k=i;
           break;  
       }
        if (num>10)  {num=10;}
        //if (NUM[num][i]) {OpenOutputPortAo3(i);} else {CloseOutputPortAo3(i);}
        if (NUM[num][i]) {OpenOutputPortAo3(k);} else {CloseOutputPortAo3(k);}
     }

}

void update7SEG(int index)
{
    int i=0;
    for (i=0;i<NO_OF_LED_TO_SCAN;i++) {
        CloseOutput(portScan[i]);
    }
    display7SEG(led_buffer[index]);
    OpenOutput(portScan[index]);
}
void UpdateBuffer(int a,int b)
{
	led_buffer[0]=a/10;
	led_buffer[1]=a%10; // 2 den cua hang doc
    led_buffer[2]=b/10;
    led_buffer[3]=b%10;
}

void ledScan()
{   
    update7SEG(led_index++);
    if (led_index>=NO_OF_LED_TO_SCAN) {led_index=0; }	
}
void resetScan()
{
    OpenOutputE();
    CloseOutputE();
}
