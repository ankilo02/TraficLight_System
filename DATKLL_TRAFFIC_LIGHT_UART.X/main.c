#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Noi khai bao hang so

// BUTTON
#define BUT1                0
#define BUT4                4
#define BUT5                5

#define PORTD_OUT   PORTD
#define TRISD_OUT   TRISD

void init_output(void)
{
   TRISD_OUT = 0x00;
    PORTD_OUT = 0x00;
}

// DURATION
int YELLOW1_NORMAL     = 2;
int GREEN1_NORMAL      = 4;
int YELLOW2_NORMAL     = 2;
int GREEN2_NORMAL      = 4;

int YELLOW1_CAODIEM   =  1;
int GREEN1_CAODIEM     = 4;
int YELLOW2_CAODIEM   =  1;
int GREEN2_CAODIEM     = 4;


// MODE
#define TURNOFF             -1
#define OVERLAPWARN         -2

#define NORMAL              1
#define MODIFY_GREEN_1      2
#define MODIFY_YELLOW_1     3
#define MODIFY_GREEN_2      4
#define MODIFY_YELLOW_2     5
#define CAO_DIEM            6

#define ONLY_RED            8
#define ONLY_YELLOW         9
#define ONLY_GREEN          10
#define FLASH_YELLOW        11

#define MENU                12

#define MENU_PAGE_1         70
#define MENU_PAGE_2         71
#define MENU_PAGE_3         72
#define MENU_PAGE_4         73

#define MENU_TRAFFIC_LIGHT  13
#define MENU_TRAFFIC_LIGHT_PAGE_1  74
#define MENU_TRAFFIC_LIGHT_PAGE_2  75

#define MODIFY_GLOBALTIME   17

#define MENU_TIME_PAGE_1     76
#define MENU_TIME_PAGE_2     77
#define MODIFY_CAODIEM      15
#define MODIFY_CANHBAO      16

#define MANUAL              18


// TRAFFIC LIGHT STATE
enum TRAFFIC_STATE {
    GREEN_1,
    GREEN_2,
    YELLOW_1, 
    YELLOW_2
};
enum TRAFFIC_STATE states  = GREEN_2;

// Noi khai bao bien toan cuc
int mode                    = NORMAL;

int timeDuration1           = 0;
int timeDuration2           = 0;

int GREEN1_DURATION         = 0;
int GREEN2_DURATION         = 0;
int YELLOW1_DURATION        = 0;
int YELLOW2_DURATION        = 0;

int CAODIEM_HOUR            = 0;
int CAODIEM_MINUTE          = 0;
int CANHBAO_HOUR            = 0;
int CANHBAO_MINUTE          = 0;

int edit_selector = 0; // dung de chon trang nh?
int time_in_state   = 0;

enum MODIFY_STATE {
    caodiem,
    normal
};
enum MODIFY_STATE mod_state = normal;


// ------------ FUNCTION --------------- //
// Khai bao cac ham co ban IO
void init_system(void);
void delay_ms(int value);
void init_traffic();
void init_globalTime();

// Ham hien thi LCD
void displayTime(int a, int b);                // display a, b len LCD
void displayTime2(int a, int b, int c);        // display a, b, c len LCD
void displayTime3(int a, int b, int c, int d); // display a, b, c, d len LCD

int flash_counter=0;
void flashTwoNumer(int row,int begin, int number);// dung de blinky number

void setting_after_changing_mode(int changed_mode); //goi khi co thay doi
void button_processing(); // xu ly nut nhan 1/4/5
void TrafficLight_display(); // hien thi den giao thong tuy tung mode
void NormalMode_fsm(void); // mode chay tu do


// ------------------GLOBAL Time------------------- //
int tick_counter = 1; // do duoc +1 moi 50ms nen dem toi tick_counter = 20 se dc 1s
long sec_counter = 0; // thoi gian global
long sec_counter_checker=0; //check de in thoi gian ra lcd neu co thay doi
int prev_mode = NORMAL; // check xem co chuyen tu menu ve NORMAL hay khong de hien thi thoi gian ra lcd
long cd_begin = 60;
long cd_end = 90;
long warn_begin = 20;
long warn_end = 40;
int isModifyingFlag = 0; // dung thoi gian khi co chinh sua

void displayGlobalTime (long total_sec); // hien thi thoi gian theo format hh:mm:ss
void globalTime_process(); // dung de dem 1s dua tren interrupt 50ms
void globalTime_changemode(); // ham duoc goi de kiem tra chuyen che do den vao pha cuoi cua den giao thong, hoac sau khi flash den vang khi o mode canh bao
void modifying_globalTime(int amount); // them amount vao thoi gian global

void changeModeDependOnCurrenGlobalTime(); // kiem tra thoi gian hien tai de chuyen che do den, duoc goi trong ham globalTime_changemode()
void setLightDurationOnModify() ; // trong che do chinh thoi gian den, hien thi thoi gian den duoc chon de modify.

int overlapCheck(); // check tinh hop le trong chinh sua thoi gian
void displayglobalTime_onModifytrafficTime (long begin, long end); // trong modify thoi gian bat dau ket thuc, hien thi thoi gian bat dau ket thuc
void globaltime_modifyTrafficTimeMode(long amount, long *target); // them amount vao bien thoi gian bat dau-ket thuc.

int MANUAL1 = 1; //1 la red, 2 la green,3 la yellow
int MANUAL2 = 1;
void setLightManual();
void UartDataReceiveProcess();
////////////////////////////////////////////////////////////////////
//Hien thuc cac chuong trinh con, ham, module, function duoi cho nay
////////////////////////////////////////////////////////////////////
void main(void)
{
    init_system();
    init_traffic();
    init_globalTime();  
    globalTime_changemode();
    while (1)
    {    
        if (flag_timer1 && mode != TURNOFF) {
            ledScan();
            flag_timer1 = 0;
            
        }
    
        if(flag_timer3) {
            init_uart();
            flag_timer3 = 0;
            globalTime_process();

            scan_key_matrix();
            button_processing();
            TrafficLight_display();
            
            if (sec_counter != sec_counter_checker || prev_mode != mode ) {
                displayGlobalTime(sec_counter); // chi hien thi global time khi o mode NORMAL, RUSH-HOUR, BLINK
                sec_counter_checker = sec_counter;
                prev_mode = mode;
            }
            DisplayLcdScreen();

        }
        UartDataReceiveProcess();
    }
}

// Hien thuc cac module co ban cua chuong trinh
void delay_ms(int value)
{
    int i,j;
    for(i = 0; i < value; i++)
        for(j = 0; j < 238; j++);
}

void init_system(void)
{
    //init_key_matrix_with_uart_i2c();
    
    //init_i2c();
    LcdClearS();
    TRISB = 0x00;		
    TRISD = 0x00;
    TRISA = 0x00; 
    TRISE = 0x00;
    PORTD = 0x00;
    init_output();
    
    init_lcd();
    setPortAo2(~0);
    //LED = 0x00;
    init_interrupt();
    
    setPortAo(0x00);
   
    init_timer0(4695);//dinh thoi 1ms sai so 1%
    init_timer1(46950);//dinh thoi 10ms
    init_timer3(4695);//dinh thoi 1ms
    
    SetTimer0_ms(1000);
    SetTimer1_ms(1);
    SetTimer3_ms(500); 
    delay_ms(1000);
    init_uart();
    init_key_matrix();
}

void init_traffic() {
    LcdClearS();
    displayGlobalTime(sec_counter);
 
    time_in_state = 0;
   
    switch (mode)
    {
    case NORMAL:
        LcdPrintStringS(0, 0, "RED1 GRE2 NORMAL");
        GREEN1_DURATION     = GREEN1_NORMAL;
        GREEN2_DURATION     = GREEN2_NORMAL;
        YELLOW1_DURATION    = YELLOW1_NORMAL;
        YELLOW2_DURATION    = YELLOW2_NORMAL;
        break;
    
    case CAO_DIEM:
        LcdPrintStringS(0, 0, "RED1 GRE2 RUSH-H");
        GREEN1_DURATION     = GREEN1_CAODIEM;
        GREEN2_DURATION     = GREEN2_CAODIEM;
        YELLOW1_DURATION    = YELLOW1_CAODIEM;
        YELLOW2_DURATION    = YELLOW2_CAODIEM;
        break;
                   
    default:
        LcdPrintStringS(0, 0, "RED1 GRE2 NORMAL");
        GREEN1_DURATION     = GREEN1_NORMAL;
        GREEN2_DURATION     = GREEN2_NORMAL;
        YELLOW1_DURATION    = YELLOW1_NORMAL;
        YELLOW2_DURATION    = YELLOW2_NORMAL;
        break;
    }

    states = GREEN_2;
    SetTimer0_ms(1000);
    setPortAo2(0b100001);
    
    displayTime(GREEN2_DURATION + YELLOW2_DURATION - time_in_state, GREEN2_DURATION - time_in_state);
}

void displayTime(int a, int b) {
    UpdateBuffer(a, b); // buffer for display 7LED
    timeDuration1 = a;
    timeDuration2 = b;
    
    LcdPrintNumS(1, 0,  a/10);
    LcdPrintNumS(1, 1,  a%10);
    LcdPrintNumS(1, 5, b/10);
    LcdPrintNumS(1, 6, b%10);
}

void displayTime2(int a, int b, int c) {
    LcdPrintNumS(1, 2   ,a/10);
    LcdPrintNumS(1, 3   ,a%10);
    LcdPrintNumS(1, 7   ,b/10);
    LcdPrintNumS(1, 8   ,b%10);
    LcdPrintNumS(1, 12  ,c/10);
    LcdPrintNumS(1, 13  ,c%10);
}
void displayTime3(int a, int b, int c, int d) {
    LcdPrintNumS(1, 0,  a/10);
    LcdPrintNumS(1, 1,  a%10);
    LcdPrintNumS(1, 4,  b/10);
    LcdPrintNumS(1, 5,  b%10);
    LcdPrintNumS(1, 10,  c/10);
    LcdPrintNumS(1, 11,  c%10);
    LcdPrintNumS(1, 14, d/10);
    LcdPrintNumS(1, 15, d%10);
}

void flashTwoNumer(int row,int begin, int number){
    if (flag_timer0) {
        flash_counter ++;
        if (flash_counter >= 2) {
            LcdPrintNumS(row, begin, number/10);
            LcdPrintNumS(row, begin+1, number%10);
            flash_counter = 0;
        } else {
            LcdPrintStringS(row, begin,"  ");
        }
    }
}

void displayGlobalTime (long total_sec) {
    if (mode == NORMAL || mode == FLASH_YELLOW || mode == CAO_DIEM || mode == MODIFY_GLOBALTIME ) { // chi hien thi thoi gian global o 3 mode nay
        int sec = total_sec % 60;
        int minute = total_sec%(3600) /60; 
        int hour = total_sec%(86400) /(3600); 
        LcdPrintNumS(1, 14, sec/10);
        LcdPrintNumS(1, 15, sec%10);
        LcdPrintStringS(1, 13, ":");
        LcdPrintNumS(1, 11, minute/10);
        LcdPrintNumS(1, 12, minute%10);
        LcdPrintStringS(1, 10, ":");
        LcdPrintNumS(1, 8, hour/10);
        LcdPrintNumS(1, 9, hour%10);
    }

}


void init_globalTime() {
    tick_counter = 0;
    sec_counter = 0;
    displayGlobalTime(sec_counter);
}

void globalTime_process() {
    if (isModifyingFlag == 0) {
        if (++tick_counter >= 20) {
            sec_counter++;
            tick_counter = 0;
            //UartSendString("dsds");
            if (sec_counter >= 86400) {
                sec_counter = 0;
            }
        }
    }
}

void globalTime_changemode() {
    
    switch (mode) {
        case NORMAL:{}
        case CAO_DIEM:
            changeModeDependOnCurrenGlobalTime();
            setting_after_changing_mode(mode);
            break;
        case FLASH_YELLOW:
            changeModeDependOnCurrenGlobalTime();
            if (mode != FLASH_YELLOW) {
                setting_after_changing_mode(mode);
            }               
            break;
    }     
}

//void myStrcat(char *a, char *b, char *temp)
//{
//    int m = strlen(a);
//    int n = strlen(b);
//    int i;
//    for (int i = 0; i <= m; i++)
//        temp[i] = a[1];
//    for (int i = 0; i <= n; i++)
//       temp[m+i] = b[i];
//}

void uart_sending(char *str1, const rom char *str2){
    UartSendString("!");
    UartSendString(str1);
    UartSendNum(timeDuration1);
    UartSendString(str2);
    UartSendNum(timeDuration2);
    UartSendString("#\n");
    //char temp2[3] = "12356";
    //char *temp = temp2;
    //strcat(temp, str1);
    //char temp = "!" + str1 + ":" + timeDuration1 + ":" + str2 + ":" + timeDuration2 + "#";
    //myStrcat(str1, str2, temp);
    //UartSendString(temp);
    //UartSendString(str1);
    //UartSendString("\r\n");
}

void UartDataReceiveProcess()
{
    if(flagOfDataReceiveComplete == 1)
    {
        flagOfDataReceiveComplete = 0;
        MANUAL1 = 1;
        MANUAL2 = 1;
        UpdateBuffer((int)0, (int)0); // buffer for display 7LED
        //UartSendString("!LIGHT:1:4#");
        mode = MANUAL;
        setting_after_changing_mode(mode);
        UartSendString("!LIGHT:1:4#");
    }
    if(flagOfDataReceiveComplete == 2)
    {
        
        MANUAL1++;
        if (MANUAL1 > 3) { MANUAL1 = 1;}
        setLightManual();
        flagOfDataReceiveComplete = 0;
    }
    if(flagOfDataReceiveComplete == 3)
    {
        //flagOfDataReceiveComplete = 1;
        MANUAL2++;
        if (MANUAL2 > 3) { MANUAL2 = 1;}
        setLightManual();
        flagOfDataReceiveComplete = 0;
    }
    if(flagOfDataReceiveComplete == 4)
    {
        isModifyingFlag=0;
        changeModeDependOnCurrenGlobalTime();
        setting_after_changing_mode(mode);  
        flagOfDataReceiveComplete = 0;
    }
}

int light_state = 0;

void NormalMode_fsm(void)
{
    switch (states)
    {
        case GREEN_2: {
            if (flag_timer0)
            {
                time_in_state++;
                flag_timer0 = 0;
                
                switch (mode)
                {
                case NORMAL:
                    LcdPrintStringS(0,0,"RED1 GRE2 NORMAL");
                    break;
                
                case CAO_DIEM:
                    LcdPrintStringS(0,0,"RED1 GRE2 RUSH-H");
                    break;
                
                default:
                    break;
                }

                displayTime(GREEN2_DURATION + YELLOW2_DURATION - time_in_state, GREEN2_DURATION - time_in_state);
                //uart_sending("RED1: ", " GRE2: ");
                if (light_state != 16){
                    UartSendString("!LIGHT:1:6#");
                    light_state = 16;
                }
                if (time_in_state == GREEN2_DURATION)
                {
                    
                    setPortAo2(0b010001);
                    states = YELLOW_2;
                    time_in_state = 0;
                    
                    switch (mode)
                    {
                    case NORMAL:
                        LcdPrintStringS(0,0,"RED1 YEL2 NORMAL");
                        break;
                    
                    case CAO_DIEM:
                        LcdPrintStringS(0,0,"RED1 YEL2 RUSH-H");
                        break;
                   
                    default:
                        break;
                    }
                    displayTime(YELLOW2_DURATION - time_in_state, YELLOW2_DURATION - time_in_state);
                    //uart_sending("RED1: ", " YEL2: ");
                    //UartSendString("!LIGHT:1:5#");
                    if (light_state != 15){
                        UartSendString("!LIGHT:1:5#");
                        light_state = 15;
                    }
                }
            }
            break;
        }
        case YELLOW_2: {         
            if (flag_timer0)
            {
                time_in_state++;
                flag_timer0 = 0;
                
                switch (mode)
                {
                case NORMAL:
                    LcdPrintStringS(0,0,"RED1 YEL2 NORMAL");
                    break;
                
                case CAO_DIEM:
                    LcdPrintStringS(0,0,"RED1 YEL2 RUSH-H");
                    break;
                
                default:
                    break;
                }

                displayTime(YELLOW2_DURATION - time_in_state, YELLOW2_DURATION - time_in_state);
                //uart_sending("RED1: ", " YEL2: ");
                //UartSendString("!LIGHT:1:5#");
                if (light_state != 15){
                    UartSendString("!LIGHT:1:5#");
                    light_state = 15;
                }
                if (time_in_state == YELLOW2_DURATION)
                {                
                    time_in_state = 0;
                    states = GREEN_1;
                    
                    setPortAo2(0b001100);

                    switch (mode)
                    {
                    case NORMAL:
                        LcdPrintStringS(0,0,"GRE1 RED2 NORMAL");
                        break;
                    
                    case CAO_DIEM:
                        LcdPrintStringS(0,0,"GRE1 RED2 RUSH-H");
                        break;
                   
                    default:
                        break;
                    }
                    displayTime(GREEN1_DURATION - time_in_state, GREEN1_DURATION + YELLOW1_DURATION - time_in_state);
                    //uart_sending("GRE1: ", " RED2: ");
                    //UartSendString("!LIGHT:3:4#");
                    if (light_state != 34){
                        UartSendString("!LIGHT:3:4#");
                        light_state = 34;
                    }
                }
            }
            break;
        }
        case GREEN_1: {
            if (flag_timer0)
            {
                time_in_state++;
                flag_timer0=0;
                    
                switch (mode)
                {
                case NORMAL:
                    LcdPrintStringS(0,0,"GRE1 RED2 NORMAL");
                    break;
                
                case CAO_DIEM:
                    LcdPrintStringS(0,0,"GRE1 RED2 RUSH-H");
                    break;
                
                default:
                    break;
                }
                displayTime(GREEN1_DURATION - time_in_state, GREEN1_DURATION + YELLOW1_DURATION - time_in_state);
                //uart_sending("GRE1: ", " RED2: ");
                //UartSendString("!LIGHT:3:4#");
                if (light_state != 34){
                    UartSendString("!LIGHT:3:4#");
                    light_state = 34;
                }
                if (time_in_state == GREEN1_DURATION)
                {
                    setPortAo2(~0b110101);
                    states=YELLOW_1;
                    time_in_state=0;
                    
                    switch (mode)
                    {
                    case NORMAL:
                        LcdPrintStringS(0,0,"YEL1 RED2 NORMAL");
                        break;
                    
                    case CAO_DIEM:
                        LcdPrintStringS(0,0,"YEL1 RED2 RUSH-H");
                        break;
                    
                    default:
                        break;
                    }
                    displayTime(YELLOW1_DURATION - time_in_state, YELLOW1_DURATION - time_in_state);
                    //uart_sending("YEL1: ", " RED2: ");
                    //UartSendString("!LIGHT:2:4#");
                    if (light_state != 24){
                        UartSendString("!LIGHT:2:4#");
                        light_state = 24;
                }
                }
            }

            break;
        } 
        case YELLOW_1: {
        
            if (flag_timer0)
            {
                time_in_state++;
                flag_timer0 = 0;
                    
                switch (mode)
                {
                case NORMAL:
                    LcdPrintStringS(0,0,"YEL1 RED2 NORMAL");
                    break;
                
                case CAO_DIEM:
                    LcdPrintStringS(0,0,"YEL1 RED2 RUSH-H");
                    break; 

                default:
                    break;
                }
                displayTime(YELLOW1_DURATION - time_in_state, YELLOW1_DURATION - time_in_state);
                
                if (time_in_state == YELLOW1_DURATION)
                {
                    globalTime_changemode();
                }
                //uart_sending("YEL1: ", " RED2: ");
                //UartSendString("!LIGHT:2:4#");
                if (light_state != 24){
                    UartSendString("!LIGHT:2:4#");
                    light_state = 24;
                }
            }
            break;
        }
    }
}


void setting_after_changing_mode(int changed_mode) {
    switch (changed_mode)
    {
    case MENU_PAGE_1:
        LcdClearS();
        UpdateBuffer((int)0, (int)0);
        LcdPrintStringS(0,2,"SETTING (1/4)");
        LcdPrintStringS(1,0,"TRAFFIC DURATION");
        setPortAo2(~0b111111);
        break;    
    case MENU_PAGE_2:
        LcdClearS();
        UpdateBuffer((int)0, (int)0);
        LcdPrintStringS(0,2,"SETTING (2/4)");
        LcdPrintStringS(1,3,"GLOBAL TIME");
        setPortAo2(~0b111111);
        break;    
    case MENU_PAGE_3:
        LcdClearS();
        UpdateBuffer((int)0, (int)0);
        LcdPrintStringS(0,2,"SETTING (3/4)");
        LcdPrintStringS(1,2,"SCHEDULE TIME");
        setPortAo2(~0b111111);
        break;    
    case MENU_PAGE_4:
        LcdClearS();
        UpdateBuffer((int)0, (int)0);
        LcdPrintStringS(0, 2,"SETTING (4/4)");
        LcdPrintStringS(1, 3,"MANUAL MODE");
        setPortAo2(~0b111111);
        break;

    case MENU_TRAFFIC_LIGHT_PAGE_1:
        LcdClearS();
        UpdateBuffer((int)0, (int)0);
        LcdPrintStringS(0,1,"DURATION (1/2)");
        LcdPrintStringS(1,3,"NORMAL MODE");
        setPortAo2(~0b111111);
        break;
        
    case MENU_TRAFFIC_LIGHT_PAGE_2:
        LcdClearS();
        UpdateBuffer((int)0, (int)0);
        LcdPrintStringS(0,1,"DURATION (2/2)");
        LcdPrintStringS(1,3,"RUSH-H MODE");
        setPortAo2(~0b111111);
        break;
    case MENU_TIME_PAGE_1:
        LcdClearS();
        UpdateBuffer((int)0, (int)0);
        LcdPrintStringS(0,1,"SCHEDULE (1/2)");
        LcdPrintStringS(1,4,"RUSH HOUR");
        setPortAo2(~0b111111);
        break;
    case MENU_TIME_PAGE_2:
        LcdClearS();
        UpdateBuffer((int)0, (int)0);
        LcdPrintStringS(0, 1,"SCHEDULE (2/2)");
        LcdPrintStringS(1, 3,"BLINKY TIME");
        setPortAo2(~0b111111);
        break;
    
    case MODIFY_CAODIEM:
        LcdClearS();
        UpdateBuffer((int)0, (int)0);
        edit_selector = 0;
        displayglobalTime_onModifytrafficTime(cd_begin,cd_end);
        setPortAo2(~0b111111);
        break;

    case MODIFY_CANHBAO:
        LcdClearS();
        LcdPrintStringS(0, 0, "START:");
        LcdPrintStringS(1, 0, "END:");
        UpdateBuffer((int)0, (int)0);
        edit_selector = 0;
        displayglobalTime_onModifytrafficTime(warn_begin,warn_end);
        setPortAo2(0b0);
        break;

    case NORMAL:
        isModifyingFlag=0;
        setPortAo2(0b0);
        init_traffic();
        break;

    case MODIFY_GREEN_1:
        LcdClearS();
        LcdPrintStringS(0,0,"GRE1");
        setLightDurationOnModify ();
        displayTime3(   GREEN1_DURATION, YELLOW1_DURATION, 
                        GREEN2_DURATION, YELLOW2_DURATION);
        UpdateBuffer(GREEN1_DURATION, GREEN1_DURATION);
        setPortAo2(0b0);
        
        break;

    case MODIFY_YELLOW_1:
        LcdClearS();
        LcdPrintStringS(0,4,"YEL1");
        displayTime3(   GREEN1_DURATION, YELLOW1_DURATION, 
                        GREEN2_DURATION, YELLOW2_DURATION);
        setPortAo2(0b0);
        UpdateBuffer(YELLOW1_DURATION, YELLOW1_DURATION);
        
        break;

    case MODIFY_GREEN_2:
        LcdClearS();
        LcdPrintStringS(0,10,"GRE2");
        setLightDurationOnModify ();
        displayTime3(   GREEN1_DURATION, YELLOW1_DURATION, 
                        GREEN2_DURATION, YELLOW2_DURATION);
        setPortAo2(0b0);
        UpdateBuffer(GREEN2_DURATION, GREEN2_DURATION);
        
        break;
    
    case MODIFY_YELLOW_2:
        LcdClearS();
        LcdPrintStringS(0,12,"YEL2");
        setLightDurationOnModify ();
        displayTime3(   GREEN1_DURATION, YELLOW1_DURATION, 
                        GREEN2_DURATION, YELLOW2_DURATION);
        setPortAo2(0b0);
        UpdateBuffer(YELLOW2_DURATION, GREEN2_DURATION);
        
        break;

    case CAO_DIEM:
        setPortAo2(0b0);
        init_traffic();
        break;

    case FLASH_YELLOW:
        LcdClearS();
        LcdPrintStringS(0,10,"BLINKY");
        UpdateBuffer((int)0, (int)0);
        setPortAo2(0b0);
        break;

    case MODIFY_GLOBALTIME:
        isModifyingFlag=1;
        LcdClearS();
        edit_selector = 0;
        LcdPrintStringS(0, 3, "MODIFY TIME");
    break;
    case MANUAL:
        LcdClearS();
        setPortAo2(0b001001);
        LcdPrintStringS(0,0,"STREET 1: RED");
        LcdPrintStringS(1,0,"STREET 2: RED");
        MANUAL1=1;
        MANUAL2=1;
    break;
   case TURNOFF:
        setPortAo2(0b111111); // no ~
        PORTD=0;
        display7SEG(10);
        isModifyingFlag=0;
        LcdClearS();  
        break;
    case OVERLAPWARN:
        LcdClearS();
        LcdPrintStringS(0,5,"INVALID");
        LcdPrintStringS(1,5,"SETTING");
        SetTimer0_ms(3000);
        break;
    default:
        break;
    }
}

void button_processing() {
    // BUTTON1 IS PRESSED 
    if (key_code[BUT1] == 60) {
            if (mode == TURNOFF) {
                    changeModeDependOnCurrenGlobalTime();
                    setting_after_changing_mode(mode); 
                } else {
                    mode = TURNOFF;
                    setting_after_changing_mode(mode);
                }
    }
    

    if (key_code[BUT1] == 1 && mode != TURNOFF) {     
        if ( mode == MODIFY_CANHBAO ||  mode == MODIFY_CAODIEM ) {
            if (overlapCheck()==0) { 
                mode= OVERLAPWARN;  
                setting_after_changing_mode(mode); 
                return;
            }     
        }
        if (mode < MENU_PAGE_1 || mode > MENU_PAGE_4) {
            mode = MENU_PAGE_1;
            setting_after_changing_mode(mode);
            return;
        } else { 
            isModifyingFlag=0;
            changeModeDependOnCurrenGlobalTime();
            setting_after_changing_mode(mode);  
            return;
        }
    }	

    // BUTTON4 IS PRESSED 
    if (key_code[BUT4] == 1 || key_code[BUT4] >= 20) {

        switch(mode) {
            case MENU_PAGE_1:
                mode = MENU_TRAFFIC_LIGHT_PAGE_1;
                setting_after_changing_mode(mode);
                break;
            case MENU_PAGE_2:
                mode = MODIFY_GLOBALTIME;
                setting_after_changing_mode(mode);
                break;
            case MENU_PAGE_3:
                mode = MENU_TIME_PAGE_1;
                setting_after_changing_mode(mode);
                break;
            case MENU_PAGE_4:
                mode = MANUAL;
                setting_after_changing_mode(mode);
                break; 
            case MENU_TRAFFIC_LIGHT_PAGE_1:
                mode = MODIFY_GREEN_1;
                mod_state = normal ;
                setting_after_changing_mode(mode);
                break;
            case MENU_TRAFFIC_LIGHT_PAGE_2:
                mode = MODIFY_GREEN_1;
                mod_state = caodiem;
                setting_after_changing_mode(mode);
                break;
            case MODIFY_GREEN_1:
                if (GREEN1_DURATION++ >= 99) {
                    GREEN1_DURATION = 1;
                }
                UpdateBuffer(GREEN1_DURATION, GREEN1_DURATION);
                displayTime3(GREEN1_DURATION, YELLOW1_DURATION
                             ,GREEN2_DURATION, YELLOW2_DURATION);
                if (mod_state == normal) {
                    GREEN1_NORMAL = GREEN1_DURATION;
                } else {
                    GREEN1_CAODIEM = GREEN1_DURATION;
                }

                break;

            case MODIFY_YELLOW_1:
                if (YELLOW1_DURATION++ >= 99) {
                    YELLOW1_DURATION = 1;
                }
                UpdateBuffer(YELLOW1_DURATION, YELLOW1_DURATION);
                displayTime3(GREEN1_DURATION, YELLOW1_DURATION
                             ,GREEN2_DURATION, YELLOW2_DURATION);
                if (mod_state == normal) {
                    YELLOW1_NORMAL = YELLOW1_DURATION;
                } else {
                    YELLOW1_CAODIEM = YELLOW1_DURATION;
                }
                break;

            case MODIFY_GREEN_2:
                if (GREEN2_DURATION++ >= 99) {
                    GREEN2_DURATION = 1;
                }
                UpdateBuffer(GREEN2_DURATION,GREEN2_DURATION);
                displayTime3(GREEN1_DURATION, YELLOW1_DURATION
                             ,GREEN2_DURATION, YELLOW2_DURATION);               
                if (mod_state == normal) {
                    GREEN2_NORMAL = GREEN2_DURATION;
                } else {
                    GREEN2_CAODIEM = GREEN2_DURATION;
                }
                break;

            case MODIFY_YELLOW_2:
                if (YELLOW2_DURATION++ >= 99) {
                    YELLOW2_DURATION = 1;
                }
                UpdateBuffer(YELLOW2_DURATION, YELLOW2_DURATION);
                displayTime3(GREEN1_DURATION, YELLOW1_DURATION
                             ,GREEN2_DURATION, YELLOW2_DURATION);  
                if (mod_state == normal) {
                    YELLOW2_NORMAL = YELLOW2_DURATION;
                } else {
                    YELLOW2_CAODIEM = YELLOW2_DURATION;
                }
                break;


            case MENU_TIME_PAGE_1:
                mode = MODIFY_CAODIEM;
                setting_after_changing_mode(mode);
                break;
            case MENU_TIME_PAGE_2:
                mode = MODIFY_CANHBAO;
                setting_after_changing_mode(mode);
                break;
                
            case MODIFY_CAODIEM:
                switch (edit_selector) {
                    case 0: 
                        globaltime_modifyTrafficTimeMode(3600,&cd_begin);
                        break;
                    case 1: 
                        globaltime_modifyTrafficTimeMode(60,&cd_begin);
                        break;
                    case 2: 
                        globaltime_modifyTrafficTimeMode(1,&cd_begin);
                        break;
                    case 3: 
                        globaltime_modifyTrafficTimeMode(3600,&cd_end);
                        break;
                    case 4: 
                        globaltime_modifyTrafficTimeMode(60,&cd_end);
                        break;
                    case 5: 
                        globaltime_modifyTrafficTimeMode(1,&cd_end);
                        break;
                }
                displayglobalTime_onModifytrafficTime(cd_begin,cd_end);
                break;
                
            case MODIFY_CANHBAO:
               switch (edit_selector) {
                    case 0: 
                        globaltime_modifyTrafficTimeMode(3600,&warn_begin);
                        break;
                    case 1: 
                        globaltime_modifyTrafficTimeMode(60,&warn_begin);
                        break;
                    case 2: 
                        globaltime_modifyTrafficTimeMode(1,&warn_begin);
                        break;
                    case 3: 
                        globaltime_modifyTrafficTimeMode(3600,&warn_end);
                        break;
                    case 4: 
                        globaltime_modifyTrafficTimeMode(60,&warn_end);
                        break;
                    case 5: 
                        globaltime_modifyTrafficTimeMode(1,&warn_end);
                        break;
                }
                displayglobalTime_onModifytrafficTime(warn_begin,warn_end);
                break;
                
            case MODIFY_GLOBALTIME:
                switch (edit_selector) {
                    case 0:
                        modifying_globalTime(3600);
                        break;
                    case 1:
                        modifying_globalTime(60);
                        break;
                    case 2:
                        modifying_globalTime(1);
                        break;      
                }
                displayGlobalTime(sec_counter);
                break; 
            case MANUAL:
                MANUAL1++;
                if (MANUAL1 > 3) { MANUAL1 = 1;}
                setLightManual();
                break;
        }
	
        return;
    }

    // BUTTON5 IS PRESSED
    if (key_code[BUT5] == 1 || key_code[BUT5] >= 20) {
        if (mode >= MENU_PAGE_1 && mode <= MENU_PAGE_4) {
            mode++;
            if (mode > MENU_PAGE_4) {
                mode = MENU_PAGE_1;
            }
            setting_after_changing_mode(mode);
            return;
        }
        if (mode >= MENU_TRAFFIC_LIGHT_PAGE_1 && mode <= MENU_TRAFFIC_LIGHT_PAGE_2) {
            mode++;
            if (mode > MENU_TRAFFIC_LIGHT_PAGE_2) {
                mode = MENU_TRAFFIC_LIGHT_PAGE_1;
            }
            setting_after_changing_mode(mode);
            return;
        }
        
        if (mode == MODIFY_YELLOW_2) {
            mode = NORMAL;       
            setting_after_changing_mode(mode);
            return;
        } 
        else if (mode >= MODIFY_YELLOW_2 || mode< MODIFY_GREEN_1) {
            
        }
        else {
            mode++;
            setting_after_changing_mode(mode);
            return;
        }
        
        if (mode >= MENU_TIME_PAGE_1 && mode <= MENU_TIME_PAGE_2) {
            mode ++;
            if (mode > MENU_TIME_PAGE_2) {
                mode = MENU_TIME_PAGE_1;
            }
            setting_after_changing_mode(mode);            
            return;
        }
        
        switch (mode)
        {
        case MODIFY_CAODIEM:
            edit_selector++;
            displayglobalTime_onModifytrafficTime(cd_begin, cd_end);
            if (edit_selector >= 6) {
                if (overlapCheck()==0) {
                    mode = OVERLAPWARN;
                    setting_after_changing_mode(mode);
                } else {
                    mode = NORMAL;
                    setting_after_changing_mode(mode);
                }
            }
            
           break;
        case MODIFY_CANHBAO:
            edit_selector++;
            displayglobalTime_onModifytrafficTime(warn_begin,warn_end);
            if (edit_selector >= 6) {
                if (overlapCheck()==0) {
                    mode = OVERLAPWARN;
                    setting_after_changing_mode(mode);
                } else {
                    mode = NORMAL;
                    setting_after_changing_mode(mode);
                }
            }
            
            break;

        case MODIFY_GLOBALTIME:
            edit_selector++;
            displayGlobalTime(sec_counter);
            if (edit_selector >= 3) {
                mode = NORMAL;
                setting_after_changing_mode(mode);
            }        
            break;
        case MANUAL:
            MANUAL2++;
            if (MANUAL2 > 3) { MANUAL2 = 1;}
            setLightManual();
            break;       
        default:
            break;
        }

        return;
    }

}


void TrafficLight_display() {
    switch(mode) {
        case MENU_TRAFFIC_LIGHT_PAGE_1:
            break;
        case MENU_TRAFFIC_LIGHT_PAGE_2:
            break;
        case MENU_TIME_PAGE_1:
            break;
        case MENU_TIME_PAGE_2:
            break;

        case MODIFY_CAODIEM:
            LcdPrintStringS(0, 0, "START:");
            LcdPrintStringS(1, 0, "END:");
            if (flag_timer0) {
                switch (edit_selector) {
                    case 0:
                        flashTwoNumer(0,8,cd_begin % 86400 / 3600);
                        break;
                    case 1:
                        flashTwoNumer(0,11,cd_begin % 3600 / 60);
                        break;    
                    case 2:
                        flashTwoNumer(0,14,cd_begin%60);
                        break;
                    case 3:
                        flashTwoNumer(1,8,cd_end % 86400 / 3600);
                        break;
                    case 4:
                        flashTwoNumer(1,11,cd_end % 3600 / 60);
                        break;
                    case 5:
                        flashTwoNumer(1,14,cd_end % 60);
                        break;
                }
                SetTimer0_ms(500);
            }
            break;

        case MODIFY_CANHBAO:
            if (flag_timer0) {
                switch (edit_selector) {
                    case 0:
                        flashTwoNumer(0,8,warn_begin % 86400 / 3600);
                        break;
                    case 1:
                        flashTwoNumer(0,11,warn_begin % 3600 / 60);
                        break;    
                    case 2:
                        flashTwoNumer(0,14,warn_begin%60);
                        break;
                    case 3:
                        flashTwoNumer(1,8,warn_end % 86400 / 3600);
                        break;
                    case 4:
                        flashTwoNumer(1,11,warn_end % 3600 / 60);
                        break;
                    case 5:
                        flashTwoNumer(1,14,warn_end % 60);
                        break;
                }
                SetTimer0_ms(500);
            }
            break;    
        case NORMAL:
            NormalMode_fsm();           
            break;

        case MODIFY_GREEN_1:
            if (flag_timer0) {
                LcdPrintStringS(0,0,"GRE1");
                ReverseOutputPortAo2(2);
                flashTwoNumer(1,0, GREEN1_DURATION);
                SetTimer0_ms(500);
            }

            break;
        case MODIFY_YELLOW_1:
            if (flag_timer0) {
                LcdPrintStringS(0,4,"YEL1");
                ReverseOutputPortAo2(1);
                flashTwoNumer(1,4, YELLOW1_DURATION);
                SetTimer0_ms(500);
            }

            break;
        case MODIFY_GREEN_2:
            if (flag_timer0) {
                LcdPrintStringS(0, 10,"GRE2");
                ReverseOutputPortAo2(5);
                flashTwoNumer(1, 10, GREEN2_DURATION);
                SetTimer0_ms(500);
            }
            break;
            
        case MODIFY_YELLOW_2:
            if (flag_timer0) {
                LcdPrintStringS(0, 12,"YEL2");
                ReverseOutputPortAo2(4);
                flashTwoNumer(1, 14, YELLOW2_DURATION);
                SetTimer0_ms(500);
            }
            break;

        case CAO_DIEM:
            NormalMode_fsm();
            break;

        case FLASH_YELLOW:
            globalTime_changemode();
            if (flag_timer0) {
                //LcdClearS();
                Flash_yellow();
                SetTimer0_ms(1000);
            }
            break;

        case MODIFY_GLOBALTIME:
            if (flag_timer0) {
                switch (edit_selector) {
                    case 0:
                        flashTwoNumer(1, 8,(sec_counter%86400)/3600);
                        break;
                    case 1:
                        flashTwoNumer(1, 11,(sec_counter%3600)/60);
                        break;
                    case 2:
                        flashTwoNumer(1, 14,sec_counter%60);
                        break;        
                }
                SetTimer0_ms(500);
            }
            break;
        case MANUAL:
            break;
        case MENU_PAGE_1:
            break;
        case MENU_PAGE_2:
            break;
        case MENU_PAGE_3:
            break;
        case MENU_PAGE_4:
            break;         
        case TURNOFF:
            break;
        case OVERLAPWARN:
            if (flag_timer0) {
                mode = MENU_TIME_PAGE_1;
                setting_after_changing_mode(mode);
            }
            break;
        default:
            LcdClearS();
            LcdPrintStringS(0,0,"Mode:");
            LcdPrintNumS(0, 7, mode);
            LcdPrintStringS(1,0,"ERROR");
            break;
    }
}

void changeModeDependOnCurrenGlobalTime(){
    if ((sec_counter >= cd_begin && sec_counter < cd_end) || ((cd_begin > cd_end) && (sec_counter > cd_begin || sec_counter < cd_end)) ) {        
        mode = CAO_DIEM;        
    } else {
        if ((sec_counter >= warn_begin && sec_counter < warn_end) || ((warn_begin > warn_end) && (sec_counter > warn_begin || sec_counter < warn_end)) ) {
           mode = FLASH_YELLOW;
        } else {
            mode = NORMAL;
         }
     }
}

void setLightDurationOnModify () {
        if (mod_state == normal) {
            GREEN1_DURATION     = GREEN1_NORMAL;
            GREEN2_DURATION     = GREEN2_NORMAL;
            YELLOW1_DURATION    = YELLOW1_NORMAL;
            YELLOW2_DURATION    = YELLOW2_NORMAL;
        } else {
            if (mod_state == caodiem) {
            GREEN1_DURATION     = GREEN1_CAODIEM;
            GREEN2_DURATION     = GREEN2_CAODIEM;
            YELLOW1_DURATION    = YELLOW1_CAODIEM;
            YELLOW2_DURATION    = YELLOW2_CAODIEM;            
            }
        }
}

void displayglobalTime_onModifytrafficTime (long sec_begin, long sec_end) {
        
        int secBegin = sec_begin % 60;
        int minuteBegin = sec_begin%(3600) /60; 
        int secEnd = sec_end % 60;
        int minuteEnd = sec_end%(3600) /60; 
        int hourBegin = sec_begin%(86400) /(3600); 
        int hourEnd = sec_end%(86400) /(3600);
        LcdPrintNumS(0, 14, secBegin/10);
        LcdPrintNumS(0, 15, secBegin%10);
        LcdPrintNumS(0, 11, minuteBegin/10);
        LcdPrintNumS(0, 12, minuteBegin%10);
        LcdPrintStringS(0, 13, ":");
        LcdPrintStringS(0, 10, ":");
        LcdPrintNumS(0, 8, hourBegin/10);
        LcdPrintNumS(0, 9, hourBegin%10);
        
        LcdPrintNumS(1, 14, secEnd/10);
        LcdPrintNumS(1, 15, secEnd%10);
        LcdPrintNumS(1, 11, minuteEnd/10);
        LcdPrintNumS(1, 12, minuteEnd%10);
        LcdPrintStringS(1, 13, ":");
        LcdPrintStringS(1, 10, ":");
        LcdPrintNumS(1, 8, hourEnd/10);
        LcdPrintNumS(1, 9, hourEnd%10);
}

void globaltime_modifyTrafficTimeMode(long amount,long *target) {
    long new_value = *target +amount;

    switch (amount) {
        case 1:
            if (new_value%60 == 0) {
                new_value-=60;
            }
            break;
        case 60:
            if (new_value%3600/60 == 0) {
                new_value-=3600;
            }
            break;
        case 3600:
            break;
    } // reset
    if (new_value >= 86400) {
        new_value = new_value%86400;
    
    } 
    *target = new_value;
}
void modifying_globalTime(int amount) {
    sec_counter = sec_counter + amount;
    switch (amount) {
        case 1:
            if (sec_counter%60 == 0) {
                sec_counter-=60;
            }
            break;
        case 60:
            if (sec_counter%3600/60 == 0) {
                sec_counter-=3600;
            }
            break;
        case 3600:
            break;
    } // reset
    if ( sec_counter>= 86400) {
        sec_counter = sec_counter%86400;
    
    } 
}

// MANUAL 
void setLightManual() {
    LcdClearS();
    switch (MANUAL1) {
        case 1:
            LcdPrintStringS(0,0,"STREET 1: RED");
            if (MANUAL2 == 1)
                UartSendString("!LIGHT:1:4#");
            else if (MANUAL2 == 2)
                UartSendString("!LIGHT:1:6#");
            else if (MANUAL2 == 3)
                UartSendString("!LIGHT:1:5#");
            OpenOutputPortAo2(0);
            CloseOutputPortAo2(1);
            CloseOutputPortAo2(2);
            break;
        case 2:
            LcdPrintStringS(0,0,"STREET 1: GREEN");
            if (MANUAL2 == 1)
                UartSendString("!LIGHT:3:4#");
            else if (MANUAL2 == 2)
                UartSendString("!LIGHT:3:6#");
            else if (MANUAL2 == 3)
                UartSendString("!LIGHT:3:5#");
            OpenOutputPortAo2(2);
            CloseOutputPortAo2(0);
            CloseOutputPortAo2(1);
            break;
        case 3:
            LcdPrintStringS(0,0,"STREET 1: YELLOW");
            if (MANUAL2 == 1)
                UartSendString("!LIGHT:2:4#");
            else if (MANUAL2 == 2)
                UartSendString("!LIGHT:2:6#");
            else if (MANUAL2 == 3)
                UartSendString("!LIGHT:2:5#");
            OpenOutputPortAo2(1);
            CloseOutputPortAo2(0);
            CloseOutputPortAo2(2);
            break;
        default:
            break;
    }
    
        switch (MANUAL2) {
        case 1:
            LcdPrintStringS(1,0,"STREET 2: RED");
//            if (MANUAL1 == 1)
//                UartSendString("!LIGHT:1:4#");
//            else if (MANUAL1 == 2)
//                UartSendString("!LIGHT:3:4#");
//            else if (MANUAL1 == 3)
//                UartSendString("!LIGHT:2:4#");
            OpenOutputPortAo2(3);
            CloseOutputPortAo2(4);
            CloseOutputPortAo2(5);
            break;
        case 2:
            LcdPrintStringS(1,0,"STREET 2: GREEN");
//            if (MANUAL1 == 1)
//                UartSendString("!LIGHT:1:6#");
//            else if (MANUAL1 == 2)
//                UartSendString("!LIGHT:3:6#");
//            else if (MANUAL1 == 3)
//                UartSendString("!LIGHT:2:6#");
            OpenOutputPortAo2(5);
            CloseOutputPortAo2(3);
            CloseOutputPortAo2(4);
            break;
        case 3:
            LcdPrintStringS(1,0,"STREET 2: YELLOW");
//            if (MANUAL1 == 1)
//                UartSendString("!LIGHT:1:5#");
//            else if (MANUAL1 == 2)
//                UartSendString("!LIGHT:3:5#");
//            else if (MANUAL1 == 3)
//                UartSendString("!LIGHT:2:5#");
            OpenOutputPortAo2(4);
            CloseOutputPortAo2(3);
            CloseOutputPortAo2(5);
            break;
        default:
            break;
    }
}




int overlapCheck() {
    //if (cd_begin > cd_end && warn_begin > warn_end) {return 0;} // neu nhu ca cao diem va canh bao deu qua nua dem thi ko hop le
    
    //lan luot kiem tra end, begin co nam trong vung thoi gian cua che do den con lai
    if ((cd_end >= warn_begin && cd_end < warn_end) || ((warn_begin > warn_end) && (cd_end > warn_begin || cd_end < warn_end)) ) {        
        return 0 ; 
    } // kiem tra neu thoi gian cao diem end co nam trong vung thoi gian canh bao
    if ((cd_begin >= warn_begin && cd_begin < warn_end) || ((warn_begin > warn_end) && (cd_begin > warn_begin || cd_begin < warn_end)) ) {        
        return 0 ; 
    }
    if ((warn_end >= cd_begin && warn_end < cd_end) || ((cd_begin > cd_end) && (warn_end > cd_begin || warn_end < cd_end)) ) {        
        return 0 ; 
    }
    if ((warn_begin >= cd_begin && warn_begin < cd_end) || ((cd_begin > cd_end) && (warn_begin > cd_begin || warn_begin < cd_end)) ) {        
        return 0 ;
    }
    return 1;
}
