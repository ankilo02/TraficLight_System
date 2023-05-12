#include <p18f4620.h>

// Noi khai bao hang so
extern unsigned char statusReceive;
extern unsigned char flagOfDataReceiveComplete;
extern unsigned char indexOfDataReceive;
extern unsigned char numberOfDataReceive;
extern unsigned char dataReceive [50];
extern unsigned char indexOfdataSend;
extern unsigned char numberOfDataSend;
extern unsigned char dataSend [50];
extern unsigned int timeChangedataSend;

void SimulatedataSend();
void uart_isr_simulate_machine();
void DisplayDataReceive();

void Sim900();
void uart_isr_Sim900();
void Sim900_call(const rom unsigned char *strNumber);
void Sim900_message(const rom char *strNumber, const rom char *strData);
