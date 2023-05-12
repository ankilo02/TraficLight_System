#ifndef OUTPUT_H
#define	OUTPUT_H
#include <p18f4620.h>

void setPort2(int x);
void OpenOutput(int index);
void CloseOutput(int index);
void ReverseOutput(int index);
void OpenOutput2(int index);
void CloseOutput2(int index);
void ReverseOutput2(int index);
void OpenOutputE();
void CloseOutputE();

void sendData2(int,int);
void OpenOutputPortAo2(int index);
void CloseOutputPortAo2(int index);
void ReverseOutputPortAo2(int index);
void setPortAo2(int x); //(?èn gt) s? d?ng RA0-RA2 ?? ?i?u khi?u 8 port c?a 74hc595

void sendData(int,int);
void OpenOutputPortAo(int index);
void CloseOutputPortAo(int index);
void ReverseOutputPortAo(int index);
void setPortAo(int x); //(không dùng) s? d?ng RA3-RA5 ?? ?i?u khi?u 8 port c?a 74hc595

void sendData3(int,int);
void OpenOutputPortAo3(int index);
void CloseOutputPortAo3(int index);
void ReverseOutputPortAo3(int index);
void setPortAo3(int x); // (led 7 ?o?n ) s? d?ng RD0-RD2 ?? ?i?u khi?u 8 port c?a 74hc595
 

#endif	/* OUTPUT_H */

